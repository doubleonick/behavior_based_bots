/* prologue.js - lays out the prologue (content/prologue.js) as three spreads, between
   the inside cover and the map: #prologue1, #prologue2, #prologue3. No words or colours
   of its own. In the words, **bold** and __vocabulary__ (underlined) are marked. */
(function () {
  'use strict';
  const P = (window.GUIDE_PAGES = window.GUIDE_PAGES || {});
  const K = () => window.GuideKit;

  // the words, with **bold** and __underlined__ (a word to learn) marked
  function words(text) {
    return K().esc(text).replace(/\*\*(.+?)\*\*/g, '<b>$1</b>').replace(/__(.+?)__/g, '<u>$1</u>');
  }
  function block(b) {
    const e = K().esc;
    if (b.big) { return `<p class="write big">${words(b.big)}</p>`; }
    if (b.p) { return `<p class="write">${words(b.p)}</p>`; }
    if (b.art) { return `<img class="pro-art${b.small ? ' small' : ''}" src="${K().art(b.art)}" alt="${e(b.alt)}">`; }
    if (b.cell) {
      return `<div class="cellsay"><img class="pro-art" src="${K().art(b.cell)}" alt="${e(b.alt)}"><p class="write pen">${words(b.say)}</p></div>`;
    }
    if (b.checks) { return `<ul class="checks">${b.checks.map((c) => `<li>${e(c)}</li>`).join('')}</ul>`; }
    return '';
  }

  function spread(n) {
    const T = window.GUIDE.prologue, S = T.spreads[n - 1], e = K().esc;
    const back = n === 1 ? 'inside' : 'prologue' + (n - 1);
    const next = n === T.spreads.length ? 'map' : 'prologue' + (n + 1);
    const nextLabel = n === T.spreads.length ? T.toMap : T.turn;
    K().$('prologue' + n).innerHTML = `<div class="spread">
      <div class="leaf left">${S.left.map(block).join('')}
        <button class="turn back" type="button" data-go="${back}">← ${e(window.GUIDE.nav.back)}</button>
        <span class="pageno">${n * 2 - 1}</span></div>
      <div class="leaf right">${S.right.map(block).join('')}
        <button class="turn" type="button" data-go="${next}">${e(nextLabel)} →</button>
        <span class="pageno">${n * 2}</span></div>
    </div>`;
  }
  P.prologue1 = () => spread(1);
  P.prologue2 = () => spread(2);
  P.prologue3 = () => spread(3);
})();
