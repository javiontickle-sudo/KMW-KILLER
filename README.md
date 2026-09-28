# KMW TUNES v3 — Advanced vocal tuner prototype

Upgrades over v2:
- higher-resolution normalized autocorrelation pitch tracking
- sub-sample pitch estimate interpolation
- confidence tracking and weak/unvoiced rejection
- vocal level gate
- octave-jump rejection and target-note hysteresis
- smoother retune transitions
- correction amount control
- sustained-note Humanize behavior
- Signalsmith Stretch real pitch shifting + formant compensation
- reported plugin latency for DAW compensation
- live detected note / target / tracking-confidence display

This is original KMW code and UI. It targets the workflow of modern real-time vocal tuners, but it is not Slate Digital MetaTune and does not use MetaTune's proprietary DSP. Commercial parity requires compiled builds, listening tests, profiling, and iterative tuning across many voices and buffer/sample-rate configurations.
