# Building the WASM demo

The C engine (`src/*.c`, `lib/utils.c`) compiles to WebAssembly with
[Emscripten](https://emscripten.org/). `src/main.c` gained a small
`#ifdef __EMSCRIPTEN__` block at the bottom exposing ten `wasm_*` functions —
the native `main()` above it is completely untouched.

## 1. Install Emscripten (one-time)

```bash
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
./emsdk install latest
./emsdk activate latest
source ./emsdk_env.sh
```

`source ./emsdk_env.sh` only lasts for the current terminal session — run it
again (or add it to your shell profile) next time you open a new terminal.

## 2. Build

From the repo root:

```bash
mkdir -p docs/pkg
emcc -O2 -std=c11 -D_POSIX_C_SOURCE=200809L -Iinclude \
  src/main.c src/display.c src/formula_parser.c src/input_handler.c \
  src/recalculation.c src/scrolling.c src/spreadsheet.c src/command_router.c \
  lib/utils.c \
  -o docs/pkg/spreadsheet.js \
  -s MODULARIZE=1 \
  -s EXPORT_ES6=1 \
  -s EXPORT_NAME=createSpreadsheetModule \
  -s EXPORTED_FUNCTIONS=_wasm_create_sheet,_wasm_execute,_wasm_get_cell,_wasm_get_formula,_wasm_get_status,_wasm_get_elapsed,_wasm_num_rows,_wasm_num_cols,_wasm_first_row,_wasm_first_col \
  -s EXPORTED_RUNTIME_METHODS=ccall,cwrap \
  -s ENVIRONMENT=web \
  -s INITIAL_MEMORY=134217728 \
  -s INVOKE_RUN=0 \
  -lm
```

`-D_POSIX_C_SOURCE=200809L` is needed because `main.c` calls the POSIX
function `getline()`. Native glibc/macOS's libc expose it under `-std=c11`
regardless, but Emscripten's musl-based libc follows strict ISO C conformance
under `-std=c11` and hides POSIX extensions unless a feature-test macro asks
for them — without this flag you'd hit `call to undeclared function
'getline'`.

What each flag does:

- `MODULARIZE=1` — wraps the output in a factory function instead of dumping
  globals onto `window`.
- `EXPORT_ES6=1` — makes that factory function a real ES module `export
  default`, which is what lets `docs/app.js` do
  `import createSpreadsheetModule from "./pkg/spreadsheet.js"`.
  **Without this flag `MODULARIZE` alone produces CommonJS, and the browser
  throws `does not provide an export named 'default'`.**
- `EXPORT_NAME=createSpreadsheetModule` — names that factory function
  (must match the name imported in `app.js`).
- `EXPORTED_FUNCTIONS` — the C functions callable from JS. Note the leading
  underscore — that's the C symbol name as seen by the linker.
- `EXPORTED_RUNTIME_METHODS=ccall,cwrap` — exposes the helpers `app.js` uses
  to call into those functions with automatic JS↔C string/number marshalling.
- `ENVIRONMENT=web` — skip generating Node.js/worker fallback code paths.
- `INITIAL_MEMORY=134217728` (128MB) — a *fixed* heap, not growable. See
  section 4b below for why growth is deliberately avoided here; 128MB
  comfortably covers the default 100x100 sheet and reasonably large custom
  sizes from the "New Sheet" control, but an extreme custom size approaching
  the native CLI's 999x18278 cap could exhaust it (`create_sheet`'s `malloc`
  calls aren't null-checked, so that would crash rather than error cleanly —
  a pre-existing gap, not new here).
- `INVOKE_RUN=0` — **required**. Without it, Emscripten calls the compiled
  `main()` automatically on load. This project's `main()` expects two CLI
  args and then loops forever on `getline(stdin)` — in a browser there's no
  real stdin, so that loop would just hang the page. The `wasm_*` wrapper
  functions are the real entry points; `main()` is never invoked.

Run a local server (opening `index.html` directly via `file://` won't work —
`fetch`ing the `.wasm` file needs `http://`):

```bash
python3 -m http.server 8080
```

Then open `http://localhost:8080/docs/` (or `http://localhost:8080/` if your
server root is already `docs/`).

## 3. If the build fails

- **`emcc: command not found`** — you forgot to `source ./emsdk_env.sh` in
  this terminal.
- **`call to undeclared function 'getline'`** — missing
  `-D_POSIX_C_SOURCE=200809L` (see above).
- **`specified output file (docs/pkg/spreadsheet.js) is in a directory that
  does not exist`** — run `mkdir -p docs/pkg` first.
- **`SyntaxError: The requested module './pkg/spreadsheet.js' does not
  provide an export named 'default'`** — you're missing `-s EXPORT_ES6=1`.
- **Page loads but nothing renders / console shows a wasm fetch 404** —
  the browser is loading over `file://` instead of `http://`; use the
  `python3 -m http.server` step above.
- **Console shows `TextDecoder: The provided ArrayBuffer value must not be
  resizable`, or the status bar seems stuck** — this means `-s
  ALLOW_MEMORY_GROWTH=1` crept back into the build command; see section 4b.
  Use the fixed `INITIAL_MEMORY` flag instead.

## 4. Known pre-existing quirk (not introduced by this port)

`scroll()` in `src/scrolling.c` can push `bounds.first_col` (and in principle
`first_row`) negative when scrolling right/down on a sheet with exactly the
minimum visible width (10 cols/rows) — e.g. `first_col = min(first_col+20,
last_col-1)-10` evaluates to `-1` on a freshly created 10-column sheet. This
is pre-existing behavior in the native CLI too, just less visible there. The
demo's `app.js` clamps the viewport offset to `[0, dimension-1]` defensively
so it never renders a negative row/column, but the underlying quirk in
`scroll()` is unchanged — worth a look if you want to fix it upstream.

## 4b. Fixed: cycle-detection status message appeared stuck on "ok"

Creating a circular dependency (e.g. `B6=B7` then `B7=B6`) always correctly
left the target cell's value untouched (verified: no corruption, the
dependency simply wasn't added) — but the status bar kept showing `ok`
instead of `Cycle detected in dependencies`. This looked at first like the
cycle-detection logic itself was broken under `emcc`, but it wasn't:

The real cause was `-s ALLOW_MEMORY_GROWTH=1`. Once the WASM heap needed to
grow past its initial size, Chrome's `TextDecoder.decode()` started throwing
`The provided ArrayBuffer value must not be resizable` for *any*
string-returning wasm call (`wasm_get_status`, `wasm_get_formula`) — because
Emscripten's growable memory backs `HEAPU8` with a resizable `ArrayBuffer`,
which newer Chrome's `TextDecoder` rejects outright. `app.js`'s `renderAll()`
calls `wasmGetStatus()` without a try/catch, so once that started throwing,
the exception silently aborted the rest of that render pass, freezing the
status text at whatever it last successfully showed — which happened to be
"ok" from earlier in the session. The actual C-side `has_cycle()` check was
correct the entire time (confirmed directly from the browser console via
`ccall`, independent of the status display).

Fix: switched to a *fixed* `INITIAL_MEMORY` instead of
`ALLOW_MEMORY_GROWTH=1` (see the flag list above), which avoids the growable
memory path — and therefore the resizable-`ArrayBuffer` `TextDecoder`
incompatibility — entirely. No C code needed to change; the cycle-detection
logic was never the problem.

## 5. Deploying to GitHub Pages

Settings → Pages → Build and deployment → Source: "Deploy from a branch" →
Branch: `main`, folder: `/docs` → Save. The live URL will be
`https://evans-prince.github.io/Command-Line-Excel/`.
