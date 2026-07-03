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
emcc -O2 -std=c11 -Iinclude \
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
  -s ALLOW_MEMORY_GROWTH=1 \
  -s INVOKE_RUN=0 \
  -lm
```

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
- `ALLOW_MEMORY_GROWTH=1` — the sheet's grid is `rows*cols` cell structs;
  spinning up a large custom sheet from the "New Sheet" control can need more
  memory than Emscripten's default fixed heap.
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
- **`specified output file (docs/pkg/spreadsheet.js) is in a directory that
  does not exist`** — run `mkdir -p docs/pkg` first.
- **`SyntaxError: The requested module './pkg/spreadsheet.js' does not
  provide an export named 'default'`** — you're missing `-s EXPORT_ES6=1`.
- **Page loads but nothing renders / console shows a wasm fetch 404** —
  the browser is loading over `file://` instead of `http://`; use the
  `python3 -m http.server` step above.

## 4. Known pre-existing quirk (not introduced by this port)

`scroll()` in `src/scrolling.c` can push `bounds.first_col` (and in principle
`first_row`) negative when scrolling right/down on a sheet with exactly the
minimum visible width (10 cols/rows) — e.g. `first_col = min(first_col+20,
last_col-1)-10` evaluates to `-1` on a freshly created 10-column sheet. This
is pre-existing behavior in the native CLI too, just less visible there. The
demo's `app.js` clamps the viewport offset to `[0, dimension-1]` defensively
so it never renders a negative row/column, but the underlying quirk in
`scroll()` is unchanged — worth a look if you want to fix it upstream.

## 5. Deploying to GitHub Pages

Settings → Pages → Build and deployment → Source: "Deploy from a branch" →
Branch: `main`, folder: `/docs` → Save. The live URL will be
`https://evans-prince.github.io/Command-Line-Excel/`.
