/* tray.js - an observation window: a round tray with one half lit, and one creature in
   it, following Vehicle 1's rule (what it feels sets how much it moves, never where).
     Val:       one wheel, so it goes straight; the pulse drive (push and glide) turns
                what it feels into beats. It stops where it feels nothing, or at the rim,
                and a hand sets it down again facing the other half.
     woodlouse: wanders (it turns at random and at the rim), faster where it feels more.
   Colours come from the style (CSS custom properties on the canvas); no words here.
   A simulation, standing in until Val's real runs are filmed from above. */
(function () {
  'use strict';
  const SIZE = 360, R = 168, C = SIZE / 2;
  const rnd = Math.random;
  function gauss() { return Math.sqrt(-2 * Math.log(rnd() + 1e-9)) * Math.cos(2 * Math.PI * rnd()); }

  class Tray {
    // canvas: a <canvas>; who: 'val' or 'louse'; images: { val, louse } (loaded <img>s)
    constructor(canvas, who, images) {
      this.cv = canvas; this.ctx = canvas.getContext('2d'); this.who = who; this.img = images;
      canvas.width = SIZE; canvas.height = SIZE;
      this.wire = 1;          // +1: more light, more movement; -1: less
      this.side = 1;          // +1: the right half is lit; -1: the left
      this.playing = false; this.last = 0;
      this.reset();
      const css = getComputedStyle(canvas);
      this.col = {
        dark: css.getPropertyValue('--tray-dark').trim() || '#5c6676',
        lit: css.getPropertyValue('--tray-lit').trim() || '#fff6cf',
        trail: css.getPropertyValue('--tray-trail').trim() || '#E07A2E',
        rim: css.getPropertyValue('--tray-rim').trim() || '#33251a'
      };
      this.draw();
    }
    light(x) { return 1 / (1 + Math.exp(-(x - C) * this.side / 16)); }
    feel(x) { const L = this.light(x); return this.wire > 0 ? L : 1 - L; }
    inside(x, y) { return Math.hypot(x - C, y - C) < R - 14; }

    reset() {
      this.heat = new Float32Array(30 * 30); this.heatMax = 1;
      this.timeLit = 0; this.timeAll = 0;
      this.setDown();
    }
    // A hand sets the creature down: Val on the side where it moves, facing the other half
    setDown() {
      const moves = this.wire > 0 ? this.side : -this.side;   // the half where it feels a lot
      if (this.who === 'val') {
        this.x = C + moves * (40 + 50 * rnd()); this.y = C + (rnd() - 0.5) * 120;
      } else {
        // the woodlouse is let go anywhere in the tray
        const a = rnd() * Math.PI * 2, r = (R - 30) * Math.sqrt(rnd());
        this.x = C + r * Math.cos(a); this.y = C + r * Math.sin(a);
      }
      this.h = this.who === 'val' ? (moves > 0 ? Math.PI : 0) + (rnd() - 0.5) * 1.0 : rnd() * Math.PI * 2;
      this.v = 0; this.phase = 0.999; this.beat = -1; this.still = 0; this.stuck = false;
    }

    play() { if (!this.playing) { this.playing = true; this.last = performance.now(); requestAnimationFrame((t) => this.tick(t)); } }
    pause() { this.playing = false; }

    step(dt) {
      const f = this.feel(this.x);
      if (this.who === 'val') {
        // the pulse drive: beats at up to 1.6 a second; each a quick push, then easing off
        const beats = 1.6, D = 1 / beats;
        this.phase += f * beats * dt;
        if (this.phase >= 1) { this.phase -= 1; this.beat = 0; }
        let push = 0;
        if (this.beat >= 0) {
          const s = this.beat / D;
          push = s < 0.15 ? s / 0.15 : Math.exp(-(s - 0.15) * 3.2);
          this.beat += dt; if (this.beat >= D) { this.beat = -1; }
        }
        this.v = this.stuck ? 0 : 95 * push;
        this.h += 0.02 * dt;                       // a slight drift, no steering
        const nx = this.x + Math.cos(this.h) * this.v * dt, ny = this.y + Math.sin(this.h) * this.v * dt;
        if (this.inside(nx, ny)) { this.x = nx; this.y = ny; } else { this.stuck = true; }
        // stopped (nothing felt) or at the rim: after a while a hand sets Val down again
        if (this.stuck || f < 0.03) { this.still += dt; } else { this.still = 0; }
        if (this.still > 2.2) { this.setDown(); }
      } else {
        // the woodlouse: faster where it feels more; it turns at random as it goes
        this.v = 8 + 100 * f;
        this.h += gauss() * 0.8 * Math.sqrt(dt);   // fairly straight stretches, as a woodlouse walks
        const nx = this.x + Math.cos(this.h) * this.v * dt, ny = this.y + Math.sin(this.h) * this.v * dt;
        if (this.inside(nx, ny)) { this.x = nx; this.y = ny; } else { this.h = Math.atan2(C - this.y, C - this.x) + (rnd() - 0.5) * 1.6; }
      }
      // where it spends its time
      const i = Math.min(29, Math.max(0, Math.floor(this.x / 12))) + 30 * Math.min(29, Math.max(0, Math.floor(this.y / 12)));
      this.heat[i] += dt; this.heatMax = Math.max(this.heatMax, this.heat[i]);
      this.timeAll += dt; if (this.light(this.x) > 0.5) { this.timeLit += dt; }
    }
    tick(now) {
      if (!this.playing) { return; }
      // follow real time (up to a quarter second a frame), in small steps, so a slow
      // browser still runs the same number of seconds
      const dt = Math.max(0, Math.min(0.25, (now - this.last) / 1000)); this.last = now;
      const n = Math.max(1, Math.ceil(dt * 240));
      for (let k = 0; k < n; k++) { this.step(dt / n); }
      this.draw();
      requestAnimationFrame((t) => this.tick(t));
    }
    // share of time in the lit half since the last reset (0..1)
    litShare() { return this.timeAll ? this.timeLit / this.timeAll : 0; }

    draw() {
      const g = this.ctx;
      g.clearRect(0, 0, SIZE, SIZE);
      g.save(); g.beginPath(); g.arc(C, C, R, 0, Math.PI * 2); g.clip();
      // a soft edge: blend by drawing dark, then the lit half with a gradient mask
      g.fillStyle = this.col.dark; g.fillRect(0, 0, SIZE, SIZE);
      const lg = g.createLinearGradient(C - 30 * this.side, 0, C + 30 * this.side, 0);
      lg.addColorStop(0, 'rgba(0,0,0,0)'); lg.addColorStop(1, this.col.lit);
      g.fillStyle = this.col.lit; g.globalAlpha = 1;
      if (this.side > 0) { g.fillRect(C + 30, 0, SIZE, SIZE); } else { g.fillRect(0, 0, C - 30, SIZE); }
      g.fillStyle = lg; g.fillRect(C - 30, 0, 60, SIZE);
      // the time trail
      g.fillStyle = this.col.trail;
      for (let i = 0; i < 900; i++) {
        if (this.heat[i] > 0) { g.globalAlpha = 0.15 + 0.6 * this.heat[i] / this.heatMax; g.fillRect((i % 30) * 12 + 2, Math.floor(i / 30) * 12 + 2, 8, 8); }
      }
      g.globalAlpha = 1;
      // the creature
      g.save(); g.translate(this.x, this.y); g.rotate(this.h + Math.PI / 2); g.imageSmoothingEnabled = false;
      const im = this.who === 'val' ? this.img.val : this.img.louse;
      if (im && im.complete) {
        if (this.who === 'val') { g.drawImage(im, -11, -27, 22, 55); } else { g.drawImage(im, -14, -14, 28, 28); }
      }
      g.restore();
      g.restore();
      g.strokeStyle = this.col.rim; g.lineWidth = 6; g.beginPath(); g.arc(C, C, R, 0, Math.PI * 2); g.stroke();
    }
  }
  window.Tray = Tray;
})();
