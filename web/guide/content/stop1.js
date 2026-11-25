/* The guide's content for stop 1, Vehicle 1: words only. Carried over from the
   Vehicle 1 lesson mock-up (web/mockups/v1-lesson/Lesson.dc.html, whose steps and
   wording were decided 2026-11), with the jellies as the ancient stage (decided
   2026-11-10) and the pulse drive (accepted 2026-11-24).
   Anything in [square brackets] is a placeholder for the owner to write. */

window.GUIDE = window.GUIDE || {};
window.GUIDE.stop1 = {
  place: 'The Jelly Shallows',

  intro: {
    heading: 'Vehicle 1',
    sub: 'Stop 1: one sense, one push',
    story: '[The story for this stop: arriving at the Jelly Shallows. The owner writes it.]',
    jellies: [
      'Jellies have swum in the sea for more than 500 million years.',
      'A jelly has no front and no back, no left and no right: its body is round.',
      'It has one way to move: it squeezes its bell. What it feels makes it squeeze faster or slower.'
    ],
    jellyLabels: { bell: ['the bell', '(squeezes to push)'], net: ['a net of nerves', '(no brain)'], round: ['no left, no right', ''] },
    valToo: 'Val can be like that too: one sense, one wheel, no sides.',
    build: 'Build',
    buildSteps: [
      'One wheel on Val.',
      'A light sensor, pointing up at the light.',
      'A lamp fixed above one half of the floor.',
      'On Val, set it up: the setup cards show every picture Val will show you.'
    ],
    setupCards: '[The setup cards for Vehicle 1 go here.]',
    pulse: "This Val pushes in beats, like a jelly's bell: the more it feels, the more often it pushes.",
    grownUp: '[For the grown-up: Vehicle 1 uses the pulse drive (PULSE_DRIVE 1, PUSH_GLIDE in Val_Vehicle). The Handbook has the steps and why.]',
    next: 'Start: 1 · Watch'
  },

  steps: ['Watch', 'What are they doing?', 'Look inside', 'Try it'],

  watch: {
    heading: '1 · Watch',
    text: 'Val and a woodlouse both feel light, under the same pool of it. Press Play and watch where each one spends its time.',
    woodlouse: "The woodlouse is a living cousin: it came long after the jellies, but it still uses their oldest trick.",
    valWindow: 'Val',
    louseWindow: 'Specimen · woodlouse',
    play: 'Play both',
    pause: 'Pause',
    note: '[For now Val is simulated, pushing in beats; real runs of Val, filmed from above, will replace it.]',
    next: 'Next: 2 · What are they doing?'
  },

  describe: {
    heading: '2 · What are they doing?',
    text: 'Pick the word that fits best. You get two tries.',
    choices: [
      { id: 'light', label: 'Avoid light', phrase: 'avoid light',
        resp: 'Good eye: that fits best. They both end up away from the light. But does either of them ever turn away from it, or do they just end up in the dim?' },
      { id: 'love', label: 'Love', phrase: 'love the dark',
        resp: 'Interesting: they both stay a long time in one place. Is it the light they love, or the dark?' },
      { id: 'coward', label: 'Cowardice', phrase: 'act scared of the light',
        resp: 'They do rush through the bright part. But do they run away from it, or just through it?' },
      { id: 'aggro', label: 'Aggression', phrase: 'look aggressive',
        resp: 'Aggression would mean charging at something. Do either of them aim at anything?' },
      { id: 'curious', label: 'Curiosity', phrase: 'act curious',
        resp: 'They do wander all over. But is it them choosing where to go, or bumps and wobbles turning them?' }
    ],
    picked: 'You picked:',
    tryAgain: 'Try one more.',
    grownUp: '[For the grown-up: every choice sounds reasonable, and none is marked wrong. The words describe how it looks from outside. A useful question: "Does it ever turn toward or away from anything?"]',
    next: 'Next: 3 · Look inside'
  },

  inside: {
    heading: '3 · Look inside',
    ask: ['Could Val', 'with just one light sensor joined to one wheel: more light, more pushes?'],
    yes: 'Yes',
    no: 'No way',
    sensor: 'light sensor',
    wheel: 'wheel',
    answerYes: 'Yes!',
    answerNo: 'It can!',
    reveal: [
      "Where it's bright, Val pushes often and goes fast, so it doesn't stay long. Where it's dim, Val hardly pushes, so it stays a long time. Leave it running and it ends up in the dim, as if it hated the light.",
      'Nothing inside Val avoids, loves or fears anything.',
      "The woodlouse works the same way: more light, faster. It isn't afraid of anything; light just makes it hurry, and that keeps it out of the sun, where it would dry out."
    ],
    jellyHeading: 'The jellies did it first',
    jellies: [
      'A jelly that pulses more in bright light ends up in the shade. Many jellies leave the bright water near the surface by day.',
      'A jelly that pulses less in bright light ends up in the light, like the upside-down jelly, whose tiny passengers need the sun.'
    ],
    moral: "It looks complicated from outside, and it's simple inside.",
    grownUp: "[For the grown-up: Braitenberg's point: it is easier to build behaviour than to work out how it is built from watching it. The child's word describes the outside; the wiring is one line. Woodlice lose water easily, and light usually means sun and dry air. The jelly examples: surface-avoiding jellies (medium confidence for jellies in general); Cassiopea in sunny shallows.]",
    next: 'Next: 4 · Try it'
  },

  tryIt: {
    heading: '4 · Try it',
    text: 'Change one thing. Predict where the woodlouse will end up, then watch. Val runs with the same wiring beside it.',
    changes: [
      { id: 'plus', label: '+ wire', note: 'More light, more pushes.' },
      { id: 'minus', label: '− wire', note: 'More light, fewer pushes.' },
      { id: 'move', label: 'Move the light to the other side', note: 'The lamp shines on the other half now.' }
    ],
    predictAsk: 'Where will the woodlouse end up?',
    predictions: ['in the dim', 'in the light', 'all over'],
    watching: 'Watching…',
    seconds: 'of 20 seconds',
    verdictRight: 'You said {p}. It ended up {o}.',
    whyPlus: 'With a + wire, more light means more pushes: it hurries out of the light and lingers in the dim.',
    whyMinus: 'With a − wire, more light means fewer pushes: it lingers in the light and hurries through the dim.',
    whyAll: 'This time it wandered about evenly. The wobble is random: try the same thing again.',
    whyOdd: 'This time it went the other way: the wobble is random, and 20 seconds is short. Try the same thing again and see.',
    timeDim: 'It spent {n}% of its time in the dim half.',
    timeLight: 'It spent {n}% of its time in the lit half.',
    again: 'Change something else to try again.',
    notebook: 'My notebook',
    cols: ['#', 'I changed', 'I predicted', 'It ended up'],
    toMap: 'Back to the map'
  }
};
