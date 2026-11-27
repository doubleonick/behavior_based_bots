/* Lays the content (content/opening.js) out as the notebook's pages, using the art in
   art/<style>/ and the look in style/<style>.css. Holds no words of its own (they are
   all in the content) and no colours (they are all in the style).

   One view at a time, named in the address: #cover, #inside, #map, #meet-val, #button.
   The explorer's name, cover colour and progress are remembered in this browser only
   (saving that travels between machines is planned: see web/OUTLINE.md, Progress). */
(function () {
  'use strict';
  const G = window.GUIDE;
  const STYLE = 'notebook';
  // the opening's views, then any pages other files add (stop1.js adds stop 1's)
  const VIEWS = ['cover', 'inside', 'map', 'meet-val', 'button'].concat(Object.keys(window.GUIDE_PAGES || {}));
  const MAP_W = 1040, MAP_H = 840;

  // ---------- small helpers ----------
  const $ = (id) => document.getElementById(id);
  function esc(s) {
    return String(s).replace(/[&<>"]/g, (c) => ({ '&': '&amp;', '<': '&lt;', '>': '&gt;', '"': '&quot;' }[c]));
  }
  // art is named by path: "val/..." is shared by every style, the rest belongs to this style
  function art(name) {
    return name.startsWith('val/') ? 'art/' + name : 'art/' + STYLE + '/' + name;
  }
  function today() {
    return new Date().toLocaleDateString(undefined, { day: 'numeric', month: 'short', year: 'numeric' });
  }
  const params = new URLSearchParams(location.search);

  // ---------- what this browser remembers (it may forget: private windows, cleared data) ----------
  const KEY = 'val-guide';
  let saved = {};
  function load() {
    try {
      if (params.has('reset')) { localStorage.removeItem(KEY); }
      saved = JSON.parse(localStorage.getItem(KEY) || '{}') || {};
    } catch (e) { saved = {}; }
  }
  function save(changes) {
    Object.assign(saved, changes);
    try { localStorage.setItem(KEY, JSON.stringify(saved)); } catch (e) { /* not remembered: fine */ }
  }

  // ---------- the cover ----------
  function coverColour() {
    const ids = G.cover.colours.map((c) => c.id);
    return ids.includes(saved.cover) ? saved.cover : G.cover.defaultColour;
  }
  function sticker(name, slot) {
    if (name === 'val') {
      return `<div class="sticker val s${slot}"><img src="${art(G.meetVal.valArt)}" alt=""></div>`;
    }
    return `<div class="sticker s${slot}"><img src="${art('stickers/' + name + '.svg')}" alt=""></div>`;
  }
  function renderCover() {
    const v = ['mix', 'stickers', 'picture', 'journal'].includes(params.get('cover')) ? params.get('cover') : G.cover.variant;
    const t = G.title;
    let face = '';
    if (v === 'journal') {
      // a field journal: a printed label pasted on, with a small picture from the
      // prologue; the Ci pressed into the leather below. Stickers are earned later.
      const Lb = G.cover.label;
      face = `<div class="journal-label"><div class="in">
          <div class="kicker">${esc(Lb.kicker)}</div>
          <img class="vignette" src="${art(Lb.vignette)}" alt="">
          <div class="title">${esc(t.name.join(' '))}</div>
          <svg class="orn" viewBox="0 0 120 8" aria-hidden="true"><path d="M4 4 H52 M68 4 H116" stroke="currentColor" stroke-width="1"/><path d="M60 0.8 L63.2 4 L60 7.2 L56.8 4 Z" fill="currentColor"/></svg>
          <div class="who">${esc(Lb.explorer)} <span>${esc(saved.name || '')}</span></div>
          <div class="who">${esc(Lb.begun)} <span>${esc(saved.begun || '')}</span></div>
        </div></div>
        <img class="pressed-ci" src="${art('ci-bold.svg')}" alt="">`;
    } else if (v === 'picture') {
      face = `<div class="plate"><div class="pic"><img src="${art(G.cover.picture)}" alt="">
          <img class="val" src="${art(G.meetVal.valArt)}" alt="">
          <div class="words"><div class="kicker">${esc(t.kicker)}</div>
            <span class="t1">${esc(t.name[0])}</span><span class="t2">${esc(t.name[1])}</span>
            <div class="tagline">${esc(t.tagline)}</div></div></div></div>
        <img class="maker-mark" src="${art('ci-bold.svg')}" alt="">`;
    } else {
      face = `<div class="title-sticker"><span class="t1">${esc(t.name[0])}</span><span class="t2">${esc(t.name[1])}</span></div>
        <div class="pill">${esc(t.pill)}</div>`;
      if (v === 'mix') {
        face += `<div class="postcard"><img src="${art(G.cover.picture)}" alt=""></div>`;
      }
      face += G.cover.stickers.map((n, i) => sticker(n, i + 1)).join('');
    }
    const current = coverColour();
    $('cover').innerHTML = `<div class="closed">
      <div class="book" id="book">
        <span class="ribbon" aria-hidden="true"></span>
        <div class="cover" data-variant="${v}" role="img" aria-label="${esc(t.name.join(' '))}: ${esc(t.tagline)}">${face}</div>
        <span class="band" aria-hidden="true"></span>
      </div>
      <div class="colours" role="group" aria-label="${esc(G.cover.chooseColour)}"><span>${esc(G.cover.chooseColour)}</span>
        ${G.cover.colours.map((c) => `<button type="button" data-colour="${c.id}" aria-label="${esc(c.name)}" data-swatch="${c.id}" aria-pressed="${c.id === current}"></button>`).join('')}
      </div>
      <button class="btn" type="button" id="open">${esc(G.cover.open)}</button>
    </div>`;
    $('cover').querySelectorAll('[data-colour]').forEach((b) => {
      b.onclick = () => {
        save({ cover: b.dataset.colour });
        document.documentElement.dataset.cover = b.dataset.colour;
        $('cover').querySelectorAll('[data-colour]').forEach((x) => x.setAttribute('aria-pressed', x === b));
      };
    });
    $('open').onclick = () => {
      $('book').classList.add('opening');
      setTimeout(() => { location.hash = '#inside'; }, matchMedia('(prefers-reduced-motion: reduce)').matches ? 0 : 480);
    };
  }

  // ---------- inside the cover ----------
  function wordmark() {
    return `<span class="wordmark" role="img" aria-label="Curious Innovations"><img class="c" src="${art('wordmark-gear.svg')}" alt="">urious<img class="i" src="${art('wordmark-bulb.svg')}" alt="">nnovations</span>`;
  }
  function renderInside() {
    const I = G.inside;
    const promises = I.promises.map((p) => {
      const pic = p.art === 'val'
        ? `<img class="val" src="${art(G.meetVal.valArt)}" alt="">`
        : `<img src="${art(p.art)}" alt="">`;
      return `<li>${pic}<span>${esc(p.text)}</span></li>`;
    }).join('');
    $('inside').innerHTML = `<div class="spread">
      <div class="leaf left endpaper">
        <div class="printed"><div class="in">
          <div class="kicker">${esc(I.promisesKicker)}</div>
          <h2>${esc(I.promisesTitle)}</h2>
          <ul class="promises">${promises}</ul>
        </div></div>
        <button class="turn back" type="button" data-go="cover">← ${esc(G.nav.back)}</button>
      </div>
      <div class="leaf right endpaper">
        <div class="printed owner"><div class="in">
          <div class="kicker">${esc(I.belongsTo)}</div>
          <input id="explorer" type="text" autocomplete="off" maxlength="30" aria-label="${esc(I.belongsTo)}" placeholder="${esc(I.namePlaceholder)}" value="${esc(saved.name || '')}">
          <div class="row">${esc(I.begun)} <span id="begun">${esc(saved.begun || '')}</span></div>
          <div class="row">${esc(I.ifFound)} <span></span></div>
        </div></div>
        <button class="btn" type="button" data-go="prologue1">${esc(I.turn)} →</button>
        <div class="makerband">${wordmark()}</div>
      </div>
    </div>`;
    $('explorer').oninput = (e) => {
      const changes = { name: e.target.value.trim() };
      if (changes.name && !saved.begun) {
        changes.begun = today();
        $('begun').textContent = changes.begun;
      }
      save(changes);
    };
  }

  // ---------- the map ----------
  const mapW = () => (G.map.drawing ? G.map.drawing.w : MAP_W);
  const mapH = () => (G.map.drawing ? G.map.drawing.h : MAP_H);
  const u = (n) => (n * 100 / mapW()).toFixed(3) + 'cqw';   // map units -> share of the map's width
  function creatureHTML([name, dx, dy, size, turn]) {
    const pos = `left:calc(50% + ${u(dx - size / 2)});top:${u(dy - size / 2)};width:${u(size)};`;
    if (name === 'val') {
      return `<img class="creature" src="${art(G.meetVal.valArt)}" alt="" style="${pos}">`;
    }
    return `<img class="creature" src="${art('creatures/' + name + '.svg')}" alt="" style="${pos}${turn ? `transform:rotate(${turn}deg)` : ''}">`;
  }
  function renderMap() {
    const M = G.map;
    const metVal = !!saved.metVal;
    const stopAt = {};
    M.stops.forEach((s) => { stopAt[s.place] = s; });
    const places = M.places.map((p) => {
      const stop = stopAt[p.id];
      let tag = '', cls = 'place', go = '';
      if (stop && stop.id === 'meet-val') {
        cls += ' open';
        go = ' data-go="meet-val"';
        tag = metVal ? '' : `<span class="tag">${esc(M.startHere)}</span>`;
      } else if (stop && stop.n === 1 && metVal) {
        cls += ' next open';
        go = ' data-go="stop1"';
        tag = `<span class="tag">${esc(M.nextStop)}</span>`;
      }
      const label = go ? 'button' : 'div';
      return `<${label} class="${cls}"${go}${label === 'button' ? ' type="button"' : ''} style="left:${(p.x * 100 / mapW()).toFixed(2)}%;top:${(p.y * 100 / mapH()).toFixed(2)}%">
        ${p.creatures.map(creatureHTML).join('')}<span class="name">${esc(p.name)}</span>${tag}</${label}>`;
    }).join('');
    // the route is drawn only as far as the explorer has gone
    const legs = M.stops.filter((s) => s.route && metVal && s.n === 1)
      .map((s) => `<path d="M${s.route.map((pt) => pt.join(' ')).join(' L')}"/>`).join('');
    $('map').innerHTML = `<div class="maphead">
        <button type="button" data-go="prologue3">← ${esc(G.nav.back)}</button>
        <button type="button" data-go="meet-val">${esc(G.meetVal.heading)} →</button>
      </div>
      <p class="slide">${esc(M.slide)}</p>
      <div class="mapscroll"><div class="mapspread" style="aspect-ratio:${mapW()}/${mapH()}">
        <img class="geo" src="${art(M.drawing ? M.drawing.art : 'map.svg')}" alt="${esc(M.drawing ? M.drawing.alt : '')}">
        <svg class="route" viewBox="0 0 ${mapW()} ${mapH()}" aria-hidden="true">${legs}</svg>
        <div class="maptitle">${esc(M.title)}</div>
        <div class="uncharted">${esc(M.uncharted)}</div>
        ${places}
      </div></div>`;
  }

  // ---------- Meet Val ----------
  function renderMeetVal() {
    const V = G.meetVal, L = V.labels;
    const n = (cls, [a, b]) => `<span class="n ${cls}">${esc(a)}${b ? `<br><span class="pencilnote">${esc(b)}</span>` : ''}</span>`;
    $('meet-val').innerHTML = `<div class="spread">
      <div class="leaf left">
        <h2 class="head"><span>${esc(V.heading)}</span><span class="date">${esc(today())}</span></h2>
        <span class="uline" aria-hidden="true"></span>
        <p class="sub">${esc(V.sub)}</p>
        <p class="write">${esc(V.story)}</p>
        <div class="placeholder">${esc(V.tryIt)}</div>
        <label class="fill">${esc(V.predict)} <input id="predict" type="text" maxlength="60" value="${esc(saved.predict || '')}"></label>
        <button class="turn back" type="button" data-go="map">← ${esc(G.nav.back)}</button>
        <span class="pageno">7</span>
      </div>
      <div class="leaf right">
        <div class="sketch">
          <img src="${art(V.valArt)}" alt="${esc(V.valAlt)}">
          <svg viewBox="0 0 400 400" aria-hidden="true" fill="none" stroke="currentColor" stroke-width="1.8" stroke-linecap="round">
            <path d="M70 336 C110 336 140 314 156 300"/><path d="M146 298 l11 1 l-5 9"/>
            <path d="M330 120 C300 118 260 116 230 130"/><path d="M240 124 l-11 7 l11 4"/>
            <path d="M336 300 C330 280 320 262 310 248"/><path d="M306 258 l3 -11 l8 8"/>
          </svg>
          ${n('face', L.face)}${n('sockets', L.sockets)}${n('wheel', L.wheel)}
        </div>
        <p class="write">${esc(V.next)}</p>
        <button class="btn" type="button" data-go="button">${esc(V.toButton)}</button>
        <div class="slip">${esc(V.grownUp)}</div>
        <span class="pageno">8</span>
      </div>
    </div>`;
    $('predict').oninput = (e) => save({ predict: e.target.value });
  }

  // ---------- Val's button ----------
  function renderButton() {
    const B = G.button;
    const presses = B.presses.map((p) => `<div class="press"><span class="touch ${p.kind}" aria-hidden="true"><b><i></i></b></span>
        <span><b>${esc(p.name)}</b><small>${esc(p.how)}: ${esc(p.means)}</small></span></div>`).join('');
    $('button').innerHTML = `<div class="spread">
      <div class="leaf left">
        <h2 class="head"><span>${esc(B.heading)}</span></h2>
        <span class="uline" aria-hidden="true"></span>
        <p class="write">${esc(B.lede)}</p>
        <div class="presses">${presses}</div>
        <button class="turn back" type="button" data-go="meet-val">← ${esc(G.nav.back)}</button>
        <span class="pageno">9</span>
      </div>
      <div class="leaf right">
        <p class="sub">${esc(B.tryHeading)}</p>
        <div class="placeholder">${esc(B.tryIt)}</div>
        <p class="write">${esc(B.later)}</p>
        <button class="btn" type="button" id="toMap">${esc(B.toMap)}</button>
        <span class="pageno">10</span>
      </div>
    </div>`;
    $('toMap').onclick = () => { save({ metVal: true }); location.hash = '#map'; };
  }

  // ---------- one view at a time ----------
  const RENDER = Object.assign({ cover: renderCover, inside: renderInside, map: renderMap, 'meet-val': renderMeetVal, button: renderButton },
                               window.GUIDE_PAGES || {});
  function show() {
    const id = VIEWS.includes(location.hash.slice(1)) ? location.hash.slice(1) : 'cover';
    RENDER[id]();
    VIEWS.forEach((v) => { $(v).hidden = v !== id; });
    window.scrollTo(0, 0);
  }
  document.addEventListener('click', (e) => {
    const go = e.target.closest('[data-go]');
    if (go) { location.hash = '#' + go.dataset.go; }
  });

  load();
  window.GuideKit = { esc, art, today, save, saved: () => saved, $ };
  document.documentElement.dataset.cover = coverColour();
  window.addEventListener('hashchange', show);
  show();
})();
