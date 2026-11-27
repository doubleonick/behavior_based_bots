// Walks the guide from the cover, through the prologue, to the map, then through stop 1, by clicks only.
// Run: node tests/web/clickthrough.js [width]   (Playwright with Chromium; default 1100, try 390 too)
// Walk the guide by clicking only, as a reader would; fail if any step can't be clicked.
const { chromium } = require('playwright');
(async () => {
  const path = require('path');
  const dir = path.resolve(__dirname, '../../web/guide');
  const width = +process.argv[2] || 1100;
  const b = await chromium.launch(); const errs = [];
  const p = await b.newPage({ viewport: { width, height: 900 } });
  p.on('pageerror', e => errs.push(e.message));
  await p.goto('file://' + dir + '/index.html?reset'); await p.waitForTimeout(1200);
  const steps = [
    ['cover', '#open'],
    ['inside', '#inside [data-go="prologue1"]'],
    ['prologue1', '#prologue1 [data-go="prologue2"]'],
    ['prologue2', '#prologue2 [data-go="prologue3"]'],
    ['prologue3', '#prologue3 [data-go="map"]'],
    ['map', '#map .place.open'],
    ['meet-val', '#meet-val [data-go="button"]'],
    ['button', '#toMap'],
  ];
  for (const [view, sel] of steps) {
    const shown = await p.evaluate(v => !document.getElementById(v).hidden, view);
    if (!shown) { errs.push('not on ' + view); break; }
    try {
      await p.locator(sel).first().click({ timeout: 3000 });   // a real click: fails if covered
    } catch (e) {
      errs.push('could not click ' + sel + ' on ' + view);
      break;
    }
    await p.waitForTimeout(800);
  }
  const route = await p.evaluate(() => document.querySelectorAll('#map svg.route path').length);
  // then on through stop 1, by clicks: its opening, Watch, What are they doing?, Look inside, Try it
  const stop1 = [
    ['map', '#map .place.next'], ['stop1', '#stop1 [data-go="watch"]'], ['watch', '#watch [data-go="describe"]'],
    ['describe', '[data-choice="light"]'], ['describe', '#describe [data-go="look"]'],
    ['look', '[data-answer="yes"]'], ['look', '#look [data-go="try"]'], ['try', '[data-change="minus"]'],
  ];
  for (const [view, sel] of stop1) {
    const shown = await p.evaluate(v => !document.getElementById(v).hidden, view);
    if (!shown) { errs.push('not on ' + view); break; }
    try {
      await p.locator(sel).first().click({ timeout: 3000 });
    } catch (e) {
      errs.push('could not click ' + sel + ' on ' + view);
      break;
    }
    await p.waitForTimeout(500);
  }
  const predictShown = await p.evaluate(() => !!document.querySelector('#try [data-predict]'));
  if (!predictShown) { errs.push('Try it: no prediction choices after a change'); }
  await p.evaluate(() => { location.hash = '#map'; });
  await p.waitForTimeout(400);
  const end = await p.evaluate(() => location.hash);
  const ok = errs.length === 0 && end === '#map' && route === 1;   // the route to stop 1 is drawn
  console.log((ok ? 'PASS' : 'FAIL') + ' at ' + width + 'px: ended on ' + end + ', route legs ' + route + (errs.length ? ', ' + errs.join('; ') : ''));
  await b.close();
  process.exit(ok ? 0 : 1);
})();
