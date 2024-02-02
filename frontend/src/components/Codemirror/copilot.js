import {
  ViewPlugin,
  // DecorationSet,
  // EditorView,
  // ViewUpdate,
  Decoration,
  WidgetType,
  keymap,
} from "@codemirror/view";
import {
  // Text,
  Facet,
  Prec,
  StateEffect,
  StateField,
  // EditorState,
  EditorSelection,
  // TransactionSpec,
} from "@codemirror/state";


const insert = view => {
  const {txt} = view.state.field(State) || {};

  if (!txt) {
    return false;
  }

  const {state} = view;
  const {selection: {main: {head: pos}}} = state;

  view.dispatch({
    changes: {from: pos, to: pos, insert: txt},
    range: EditorSelection.cursor(pos + txt.length),
    userEvent: "input.complete",
  });

  return true;
};


const ConfigFacet = Facet.define({
  combine(value) {
    const v = value.at(-1);
    const {onChange} = v || {};
    return {
      onChange,
    };
  },
});


const State = StateField.define({
  create() {
    return {id: 1, txt: ''};
  },
  update(currentValue, txn) {
    let {txt: currentTxt} = currentValue;
    const effect = txn.effects.find(e => e.is(SuggestionEffect));
    if (txn.state.doc) {
      if (effect && effect.value.id === currentValue.id) {
        return {...currentValue, txt: currentTxt + effect.value.text};
      } else if (!txn.docChanged && !txn.selection) {
        return currentValue;
      }
    }
    return {id: currentValue.id + 1, txt: ''};
  },
});


class SuggestionWidget extends WidgetType {
  txt;
  constructor(txt) {
    super();
    this.txt = txt;
  }
  toDOM(view) {
    const span = document.createElement("span");
    span.style.opacity = "0.4";
    span.className = "cm-inline-suggestion";
    span.textContent = this.txt;
    span.onclick = insert.bind(null, view);
    return span;
  }
};

const OnRender = ViewPlugin.fromClass(
  class Plugin {
    decorations;
    constructor() {
      // Empty decorations
      this.decorations = Decoration.none;
    }
    update(update) {
      const {txt} = update.state.field(State);
      if (!txt) {
        this.decorations = Decoration.none;
        return;
      }
      //   console.log("SUGGESTION", suggestionText, update.transactions.map(t => t.effects.map(e=>e.is(InlineSuggestionEffect))));
      //   for (const tr of update.transactions) {
      //     // Check the userEvent property of the transaction
      //     if (wasAuto){
      //       wasAuto = false;
      //       debugger;
      //     }
      //     if (tr.isUserEvent("input.complete")) {
      //         console.log("Change was due to autocomplete");
      //     } else {
      //         console.log("Change was due to user input");
      //     }
      // }

      const pos = update.view.state.selection.main.head;
      const widgets = [];
      const w = Decoration.widget({
        widget: new SuggestionWidget(txt),
        side: 1,
      });
      widgets.push(w.range(pos));
      this.decorations = Decoration.set(widgets);
    }
  },
  {decorations: (v) => v.decorations},
);



const OnUpdate = ViewPlugin.fromClass(
  class Plugin {
    async update(update) {
      const {view, docChanged, focusChanged, state: {doc}} = update;
      const {id} = update.state.field(State);
      const {onChange} = view.state.facet(ConfigFacet);
      if (docChanged) {
        const str = doc.toString();
        onChange(id, str);
      }
    }
  },
);


const Keymap = Prec.highest(
  keymap.of([
    {
      key: "Tab",
      run: insert.bind(null),
    },
  ]),
);


export const SuggestionEffect = StateEffect.define();

export default ({onChange}) => [
  ConfigFacet.of({onChange}),
  State,
  OnRender,
  OnUpdate,
  Keymap,
];
