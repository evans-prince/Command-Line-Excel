// The ?v=N query string busts GitHub Pages' / browsers' asset cache — bump
// it on every change to app.js, index.html, OR a rebuilt docs/pkg/*
// (the wasm/js output changes bytes without app.js's own code changing,
// so it needs a bump too or the browser can keep serving the old build).
// Static import specifiers must be string literals (no template literals),
// so bump it by hand here, in index.html's <script src>, AND in
// ASSET_VERSION below (used for the wasm binary's own cache-busting).
const ASSET_VERSION = 3;
import createSpreadsheetModule from "./pkg/spreadsheet.js?v=3";

const INT_MIN = -2147483648; // matches C's INT_MIN, used by the engine for div-by-zero / errors

const VIEW_ROWS = 10; // matches display_sheet()'s native 10x10 terminal window
const VIEW_COLS = 10;

let Module = null;
let wasmCreateSheet, wasmExecute, wasmGetCell, wasmGetFormula, wasmGetStatus,
    wasmGetElapsed, wasmNumRows, wasmNumCols, wasmFirstRow, wasmFirstCol;

let ROWS = 100;
let COLS = 100;
let selectedRow = 0;
let selectedCol = 0;

const gridEl = document.getElementById("grid");
const statusEl = document.getElementById("statusBar");
const cellLabelEl = document.getElementById("cellLabel");
const formulaBarEl = document.getElementById("formulaBar");
const commandBarEl = document.getElementById("commandBar");
const newRowsInput = document.getElementById("newRowsInput");
const newColsInput = document.getElementById("newColsInput");

function colIndexToName(col) {
  // Mirrors lib/utils.c's col_index_to_name (bijective base-26: A, B, ... Z, AA, AB, ...)
  let name = "";
  while (col >= 0) {
    name = String.fromCharCode(65 + (col % 26)) + name;
    col = Math.floor(col / 26) - 1;
  }
  return name;
}

function cellName(row, col) {
  return colIndexToName(col) + (row + 1);
}

function clamp(v, lo, hi) {
  return Math.max(lo, Math.min(hi, v));
}

function buildGrid() {
  gridEl.innerHTML = "";
  const thead = document.createElement("tr");
  thead.appendChild(document.createElement("th"));
  for (let c = 0; c < VIEW_COLS; c++) {
    const th = document.createElement("th");
    thead.appendChild(th);
  }
  gridEl.appendChild(thead);

  for (let r = 0; r < VIEW_ROWS; r++) {
    const tr = document.createElement("tr");
    const rh = document.createElement("td");
    rh.className = "rowhead";
    tr.appendChild(rh);
    for (let c = 0; c < VIEW_COLS; c++) {
      const td = document.createElement("td");
      td.className = "cell";
      td.dataset.viewRow = r;
      td.dataset.viewCol = c;
      tr.appendChild(td);
    }
    gridEl.appendChild(tr);
  }
}

function currentOffset() {
  // The engine's scroll() has a quirk on small sheets where first_col/first_row
  // can go negative (e.g. scrolling right on a 10-col sheet) — clamp defensively
  // so the demo never tries to render a negative row/col.
  const firstRow = clamp(wasmFirstRow(), 0, Math.max(0, ROWS - 1));
  const firstCol = clamp(wasmFirstCol(), 0, Math.max(0, COLS - 1));
  return { firstRow, firstCol };
}

function renderAll() {
  const { firstRow, firstCol } = currentOffset();
  const headerRow = gridEl.rows[0];
  for (let c = 0; c < VIEW_COLS; c++) {
    const col = firstCol + c;
    headerRow.cells[c + 1].textContent = col < COLS ? colIndexToName(col) : "";
  }

  for (let r = 0; r < VIEW_ROWS; r++) {
    const row = firstRow + r;
    const tr = gridEl.rows[r + 1];
    tr.cells[0].textContent = row < ROWS ? String(row + 1) : "";

    for (let c = 0; c < VIEW_COLS; c++) {
      const col = firstCol + c;
      const td = tr.cells[c + 1];
      td.classList.remove("err", "selected");
      if (row >= ROWS || col >= COLS) {
        td.textContent = "";
        continue;
      }
      const val = wasmGetCell(row, col);
      if (val === INT_MIN) {
        td.textContent = "ERR";
        td.classList.add("err");
      } else {
        td.textContent = String(val);
      }
      if (row === selectedRow && col === selectedCol) {
        td.classList.add("selected");
      }
    }
  }

  cellLabelEl.value = cellName(selectedRow, selectedCol);
  const status = wasmGetStatus();
  const elapsed = wasmGetElapsed();
  statusEl.innerHTML =
    `<span class="${status === "ok" ? "ok" : "err"}">${status}</span>` +
    ` &nbsp;·&nbsp; ${elapsed.toFixed(1)}s &nbsp;·&nbsp; sheet ${ROWS}×${COLS}`;
}

function selectCell(row, col) {
  selectedRow = clamp(row, 0, ROWS - 1);
  selectedCol = clamp(col, 0, COLS - 1);
  const formula = wasmGetFormula(selectedRow, selectedCol);
  formulaBarEl.value = formula && formula.length > 0 ? formula : String(wasmGetCell(selectedRow, selectedCol));
  renderAll();
}

function commitFormula() {
  const value = formulaBarEl.value.trim();
  if (value.length === 0) return;
  wasmExecute(`${cellName(selectedRow, selectedCol)}=${value}`);
  renderAll();
}

function clearCell() {
  wasmExecute(`${cellName(selectedRow, selectedCol)}=0`);
  formulaBarEl.value = "0";
  renderAll();
}

function runRawCommand() {
  const cmd = commandBarEl.value.trim();
  if (cmd.length === 0) return;
  wasmExecute(cmd);
  commandBarEl.value = "";
  renderAll();
}

function scrollDir(dir) {
  wasmExecute(dir);
  renderAll();
}

function newSheet() {
  const rows = clamp(parseInt(newRowsInput.value, 10) || 100, 1, 999);
  const cols = clamp(parseInt(newColsInput.value, 10) || 100, 1, 18278);
  ROWS = rows;
  COLS = cols;
  wasmCreateSheet(rows, cols);
  // selectCell (not a manual reset + renderAll) so the formula bar picks up
  // the fresh sheet's A1 instead of showing whatever was last typed there.
  selectCell(0, 0);
}

function wireEvents() {
  gridEl.addEventListener("click", (e) => {
    const td = e.target.closest("td.cell");
    if (!td) return;
    const firstRowOffset = currentOffset().firstRow;
    const firstColOffset = currentOffset().firstCol;
    const row = firstRowOffset + Number(td.dataset.viewRow);
    const col = firstColOffset + Number(td.dataset.viewCol);
    if (row < ROWS && col < COLS) selectCell(row, col);
  });

  document.getElementById("commitBtn").addEventListener("click", commitFormula);
  formulaBarEl.addEventListener("keydown", (e) => {
    if (e.key === "Enter") commitFormula();
  });

  document.getElementById("clearBtn").addEventListener("click", clearCell);

  document.getElementById("runCommandBtn").addEventListener("click", runRawCommand);
  commandBarEl.addEventListener("keydown", (e) => {
    if (e.key === "Enter") runRawCommand();
  });

  document.querySelector(".scroll-pad .up").addEventListener("click", () => scrollDir("w"));
  document.querySelector(".scroll-pad .down").addEventListener("click", () => scrollDir("s"));
  document.querySelector(".scroll-pad .left").addEventListener("click", () => scrollDir("a"));
  document.querySelector(".scroll-pad .right").addEventListener("click", () => scrollDir("d"));

  document.getElementById("newSheetBtn").addEventListener("click", newSheet);

  document.querySelectorAll(".example").forEach((btn) => {
    btn.addEventListener("click", () => {
      if (btn.dataset.formula) {
        commandBarEl.value = btn.dataset.formula;
      } else if (btn.dataset.command) {
        commandBarEl.value = btn.dataset.command;
      }
      runRawCommand();
    });
  });
}

async function main() {
  Module = await createSpreadsheetModule({
    print: () => {},
    printErr: () => {},
    // spreadsheet.js's own internal fetch of spreadsheet.wasm uses a fixed,
    // unversioned relative path baked in at build time -- bumping the ?v=
    // on the import above doesn't touch it, so a rebuilt .wasm with the
    // same filename could keep getting served stale from cache. locateFile
    // is Emscripten's hook for redirecting that internal fetch; appending
    // ASSET_VERSION here cache-busts the wasm binary too.
    locateFile: (path, prefix) => `${prefix}${path}?v=${ASSET_VERSION}`,
  });

  wasmCreateSheet = Module.cwrap("wasm_create_sheet", null, ["number", "number"]);
  wasmExecute = Module.cwrap("wasm_execute", null, ["string"]);
  wasmGetCell = Module.cwrap("wasm_get_cell", "number", ["number", "number"]);
  wasmGetFormula = Module.cwrap("wasm_get_formula", "string", ["number", "number"]);
  wasmGetStatus = Module.cwrap("wasm_get_status", "string", []);
  wasmGetElapsed = Module.cwrap("wasm_get_elapsed", "number", []);
  wasmNumRows = Module.cwrap("wasm_num_rows", "number", []);
  wasmNumCols = Module.cwrap("wasm_num_cols", "number", []);
  wasmFirstRow = Module.cwrap("wasm_first_row", "number", []);
  wasmFirstCol = Module.cwrap("wasm_first_col", "number", []);

  wasmCreateSheet(ROWS, COLS);
  buildGrid();
  wireEvents();
  selectCell(0, 0);
}

main().catch((err) => {
  statusEl.textContent = "Failed to load wasm module: " + err;
  console.error(err);
});
