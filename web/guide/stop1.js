/* stop1.js - lays out stop 1 (Vehicle 1) from content/stop1.js: an opening spread,
   then the four steps (Watch, What are they doing?, Look inside, Try it), each a
   spread. Uses app.js's helpers (window.GuideKit) and tray.js's observation windows.
   No words or colours of its own. */
(function () {
  'use strict';
  const P = (window.GUIDE_PAGES = window.GUIDE_PAGES || {});
  const S = () => window.GUIDE.stop1;
  const K = () => window.GuideKit;
  const NAV = () => window.GUIDE.nav;

  // the images the trays draw
  let images = null;
  function trayImages() {
    if (!images) {
      images = { val: new Image(), louse: new Image() };
      images.val.src = K().art(window.GUIDE.meetVal.valArt);
      images.louse.src = K().art('creatures/woodlouse.svg');
    }
    return images;
  }
  // trays running on the page now; stopped when the page changes
  let trays = [];
  function stopTrays() { trays.forEach((t) => t.pause()); trays = []; }
  window.addEventListener('hashchange', stopTrays);

  function stepBar(current) {
    return `<ol class="stepbar">${S().steps.map((name, i) =>
      `<li class="${i + 1 < current ? 'done' : i + 1 === current ? 'now' : ''}">${i + 1} ${K().esc(name)}</li>`).join('')}</ol>`;
  }
  function head(text, dated) {
    const e = K().esc;
    return `<h2 class="head"><span>${e(text)}</span>${dated ? `<span class="date">${e(K().today())}</span>` : ''}</h2><span class="uline" aria-hidden="true"></span>`;
  }
  function slip(text) { return `<div class="slip">${K().esc(text)}</div>`; }
  function windowHTML(id, label) {
    return `<figure class="window"><canvas id="${id}" aria-label="${K().esc(label)}"></canvas><figcaption>${K().esc(label)}</figcaption></figure>`;
  }

  // ---------- the opening spread ----------
  P.stop1 = function () {
    const T = S().intro, e = K().esc, L = T.jellyLabels;
    const n = (cls, [a, b]) => `<span class="n ${cls}">${e(a)}${b ? `<br><span class="pencilnote">${e(b)}</span>` : ''}</span>`;
    K().$('stop1').innerHTML = `<div class="spread">
      <div class="leaf left">
        ${head(T.heading, true)}
        <p class="sub">${e(T.sub)}</p>
        <div class="placeholder">${e(T.story)}</div>
        ${T.jellies.map((j) => `<p class="write">${e(j)}</p>`).join('')}
        <button class="turn back" type="button" data-go="map">← ${e(NAV().back)}</button>
        <span class="pageno">11</span>
      </div>
      <div class="leaf right">
        <div class="sketch jellysketch">
          <img src="${K().art('creatures/jelly.svg')}" alt="A jellyfish: a round bell with tentacles below">
          ${n('bell', L.bell)}${n('net', L.net)}${n('round', L.round)}
        </div>
        <p class="write pen">${e(T.valToo)}</p>
        <p class="sub">${e(T.build)}</p>
        <ol class="build">${T.buildSteps.map((b) => `<li>${e(b)}</li>`).join('')}</ol>
        <div class="placeholder">${e(T.setupCards)}</div>
        <p class="pencilnote">${e(T.pulse)}</p>
        ${slip(T.grownUp)}
        <button class="btn" type="button" data-go="watch">${e(T.next)} →</button>
        <span class="pageno">12</span>
      </div>
    </div>`;
  };

  // ---------- 1 · Watch ----------
  P.watch = function () {
    const T = S().watch, e = K().esc;
    K().$('watch').innerHTML = `<div class="spread">
      <div class="leaf left">
        ${stepBar(1)}
        ${head(T.heading)}
        <p class="write">${e(T.text)}</p>
        ${windowHTML('wVal', T.valWindow)}
        <button class="turn back" type="button" data-go="stop1">← ${e(NAV().back)}</button>
        <span class="pageno">13</span>
      </div>
      <div class="leaf right">
        ${windowHTML('wLouse', T.louseWindow)}
        <p class="pencilnote">${e(T.woodlouse)}</p>
        <p><button class="btn" type="button" id="playBoth">${e(T.play)}</button></p>
        <div class="placeholder">${e(T.note)}</div>
        <button class="turn" type="button" data-go="describe">${e(T.next)} →</button>
        <span class="pageno">14</span>
      </div>
    </div>`;
    stopTrays();
    const im = trayImages();
    trays = [new window.Tray(K().$('wVal'), 'val', im), new window.Tray(K().$('wLouse'), 'louse', im)];
    const btn = K().$('playBoth');
    btn.onclick = () => {
      const on = !trays[0].playing;
      trays.forEach((t) => (on ? t.play() : t.pause()));
      btn.textContent = on ? T.pause : T.play;
    };
  };

  // ---------- 2 · What are they doing? ----------
  P.describe = function () {
    const T = S().describe, e = K().esc;
    const tries = K().saved().tries || [];
    const done = tries.length >= 2 || tries.includes('light');
    const last = T.choices.find((c) => c.id === tries[tries.length - 1]);
    const buttons = T.choices.map((c) => {
      const tried = tries.includes(c.id);
      const cls = c === last ? 'picked' : tried ? 'tried' : done ? 'spent' : '';
      return `<li><button type="button" class="choice ${cls}" data-choice="${c.id}" ${done || tried ? 'disabled' : ''}>${e(c.label)}</button></li>`;
    }).join('');
    K().$('describe').innerHTML = `<div class="spread">
      <div class="leaf left">
        ${stepBar(2)}
        ${head(T.heading)}
        <p class="write">${e(T.text)}</p>
        <ul class="choices">${buttons}</ul>
        <button class="turn back" type="button" data-go="watch">← ${e(NAV().back)}</button>
        <span class="pageno">15</span>
      </div>
      <div class="leaf right">
        ${last ? `<p class="write pen"><b>${e(T.picked)} ${e(last.label)}</b></p><p class="write">${e(last.resp)}</p>` : ''}
        ${last && !done ? `<p class="pencilnote">${e(T.tryAgain)}</p>` : ''}
        ${slip(T.grownUp)}
        ${done ? `<button class="btn" type="button" data-go="look">${e(T.next)} →</button>` : ''}
        <span class="pageno">16</span>
      </div>
    </div>`;
    K().$('describe').querySelectorAll('[data-choice]').forEach((b) => {
      b.onclick = () => { K().save({ tries: tries.concat([b.dataset.choice]), word: b.dataset.choice }); P.describe(); };
    });
  };

  // ---------- 3 · Look inside ----------
  P.look = function () {
    const T = S().inside, e = K().esc;
    const word = S().describe.choices.find((c) => c.id === K().saved().word) || S().describe.choices[0];
    const answer = K().saved().lookAnswer;
    K().$('look').innerHTML = `<div class="spread">
      <div class="leaf left">
        ${stepBar(3)}
        ${head(T.heading)}
        <p class="write">${e(T.ask[0])} <b class="pen">${e(word.phrase)}</b> ${e(T.ask[1])}</p>
        <svg class="wiring" viewBox="0 0 320 120" role="img" aria-label="${e(T.sensor)} joined by a plus wire to the ${e(T.wheel)}">
          <rect x="10" y="40" width="80" height="44" rx="6" class="part"/><text x="50" y="104" class="lbl">${e(T.sensor)}</text>
          <path d="M92 62 C150 30 180 94 236 62" class="wire"/><text x="160" y="44" class="plus">+</text>
          <circle cx="266" cy="62" r="28" class="part"/><circle cx="266" cy="62" r="6" class="hub"/><text x="266" y="110" class="lbl">${e(T.wheel)}</text>
        </svg>
        <p class="yesno"><button type="button" class="choice" data-answer="yes">${e(T.yes)}</button><button type="button" class="choice" data-answer="no">${e(T.no)}</button></p>
        <button class="turn back" type="button" data-go="describe">← ${e(NAV().back)}</button>
        <span class="pageno">17</span>
      </div>
      <div class="leaf right">
        ${answer ? `<p class="sub">${e(answer === 'yes' ? T.answerYes : T.answerNo)}</p>
          ${T.reveal.map((r) => `<p class="write">${e(r)}</p>`).join('')}
          <p class="sub">${e(T.jellyHeading)}</p>
          <div class="jellies">${T.jellies.map((j) => `<p class="write"><img src="${K().art('creatures/jelly.svg')}" alt="">${e(j)}</p>`).join('')}</div>
          <p class="write pen"><b>${e(T.moral)}</b></p>
          ${slip(T.grownUp)}
          <button class="btn" type="button" data-go="try">${e(T.next)} →</button>` : ''}
        <span class="pageno">18</span>
      </div>
    </div>`;
    K().$('look').querySelectorAll('[data-answer]').forEach((b) => {
      b.onclick = () => { K().save({ lookAnswer: b.dataset.answer }); P.look(); };
    });
  };

  // ---------- 4 · Try it ----------
  let trial = null;   // { change, prediction, startMs }
  P.try = function () {
    const T = S().tryIt, e = K().esc;
    const log = K().saved().log || [];
    const setup = K().saved().trySetup || { wire: 1, side: 1 };
    const change = trial && trial.change;
    const changeNote = change ? T.changes.find((c) => c.id === change).note : '';
    const rows = log.map((r, i) => `<tr><td>${i + 1}</td><td>${e(r.change)}</td><td>${e(r.prediction)}</td><td>${e(r.result)} ${r.right ? '✓' : '✗'}</td></tr>`).join('');
    K().$('try').innerHTML = `<div class="spread">
      <div class="leaf left">
        ${stepBar(4)}
        ${head(T.heading)}
        <p class="write">${e(T.text)}</p>
        <ul class="choices row">${T.changes.map((c) => `<li><button type="button" class="choice ${change === c.id ? 'picked' : ''}" data-change="${c.id}" ${trial && trial.running ? 'disabled' : ''}>${e(c.label)}</button></li>`).join('')}</ul>
        ${change ? `<p class="pencilnote">${e(changeNote)}</p><p class="write">${e(T.predictAsk)}</p>
          <ul class="choices row">${T.predictions.map((p) => `<li><button type="button" class="choice ${trial.prediction === p ? 'picked' : ''}" data-predict="${e(p)}" ${trial.prediction ? 'disabled' : ''}>${e(p)}</button></li>`).join('')}</ul>` : ''}
        <p class="write pen" id="tryStatus"></p>
        <button class="turn back" type="button" data-go="look">← ${e(NAV().back)}</button>
        <span class="pageno">19</span>
      </div>
      <div class="leaf right">
        <div class="pair">${windowHTML('tLouse', S().watch.louseWindow)}${windowHTML('tVal', S().watch.valWindow)}</div>
        <div id="verdict"></div>
        <p class="sub">${e(T.notebook)}</p>
        <table class="log"><thead><tr>${T.cols.map((c) => `<th>${e(c)}</th>`).join('')}</tr></thead><tbody>${rows}</tbody></table>
        <button class="btn" type="button" data-go="map">${e(T.toMap)}</button>
        <span class="pageno">20</span>
      </div>
    </div>`;
    stopTrays();
    const im = trayImages();
    const louse = new window.Tray(K().$('tLouse'), 'louse', im), val = new window.Tray(K().$('tVal'), 'val', im);
    trays = [louse, val];
    trays.forEach((t) => { t.wire = setup.wire; t.side = setup.side; t.reset(); t.draw(); });

    K().$('try').querySelectorAll('[data-change]').forEach((b) => {
      b.onclick = () => {
        const s = Object.assign({}, setup);
        if (b.dataset.change === 'plus') { s.wire = 1; }
        if (b.dataset.change === 'minus') { s.wire = -1; }
        if (b.dataset.change === 'move') { s.side = -s.side; }
        K().save({ trySetup: s });
        trial = { change: b.dataset.change };
        P.try();
      };
    });
    K().$('try').querySelectorAll('[data-predict]').forEach((b) => {
      b.onclick = () => { trial.prediction = b.dataset.predict; trial.running = true; run(); };
    });

    function run() {
      K().$('try').querySelectorAll('button.choice').forEach((x) => { x.disabled = true; });
      trays.forEach((t) => { t.reset(); t.play(); });
      const start = performance.now();
      const timer = setInterval(() => {
        const sec = Math.min(20, Math.floor((performance.now() - start) / 1000));
        const st = K().$('tryStatus');
        if (!st) { clearInterval(timer); return; }
        st.textContent = `${T.watching} ${sec} ${T.seconds}`;
        if (sec >= 20) { clearInterval(timer); finish(); }
      }, 250);
    }
    function finish() {
      trays.forEach((t) => t.pause());
      const lit = Math.round(louse.litShare() * 100), dim = 100 - lit;
      const outcome = dim >= 60 ? T.predictions[0] : lit >= 60 ? T.predictions[1] : T.predictions[2];
      const right = outcome === trial.prediction;
      const where = outcome === T.predictions[1] ? T.timeLight.replace('{n}', lit) : T.timeDim.replace('{n}', dim);
      // where the wire should take it: + wire, the dim; - wire, the light
      const expected = setup.wire > 0 ? T.predictions[0] : T.predictions[1];
      let why = setup.wire > 0 ? T.whyPlus : T.whyMinus;
      if (outcome === T.predictions[2]) { why = T.whyAll; } else if (outcome !== expected) { why = T.whyOdd; }
      const label = T.changes.find((c) => c.id === trial.change).label;
      K().save({ log: (K().saved().log || []).concat([{ change: label, prediction: trial.prediction, result: outcome, right }]) });
      K().$('verdict').innerHTML = `<p class="verdict ${right ? 'right' : 'wrong'}">${right ? '✓' : '✗'} ${e(T.verdictRight.replace('{p}', trial.prediction).replace('{o}', outcome))}</p>
        <p class="write">${e(where)} ${e(why)}</p><p class="pencilnote">${e(T.again)}</p>`;
      const tb = K().$('try').querySelector('.log tbody');
      const log2 = K().saved().log;
      tb.innerHTML = log2.map((r, i) => `<tr><td>${i + 1}</td><td>${e(r.change)}</td><td>${e(r.prediction)}</td><td>${e(r.result)} ${r.right ? '✓' : '✗'}</td></tr>`).join('');
      K().$('tryStatus').textContent = '';
      trial = null;
      K().$('try').querySelectorAll('[data-change]').forEach((x) => { x.disabled = false; x.classList.remove('picked'); });
    }
  };
})();
