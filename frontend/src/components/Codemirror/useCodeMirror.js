import React from 'react';
// import {defaultKeymap} from "@codemirror/commands"
import {closeBrackets, autocompletion} from '@codemirror/autocomplete';
import {indentWithTab} from '@codemirror/commands';
import {bracketMatching} from '@codemirror/language';
import {gutter, keymap, lineNumbers, highlightActiveLine, highlightActiveLineGutter, EditorView} from '@codemirror/view';
import {EditorState} from '@codemirror/state';
import {sql, PostgreSQL} from '@codemirror/lang-sql';
import {xcodeLight, xcodeDark} from './xcode';
import copilot, {SuggestionEffect} from './copilot';

import './codemirror.css';

const extensions = [
  // keymap.of(defaultKeymap),
  lineNumbers(),
  gutter(),
  highlightActiveLineGutter(),
  highlightActiveLine(),
  closeBrackets(),
  // autocompletion(),
  bracketMatching(),
];

export default ({id, doc, keymaps, ref, shouldUseDarkColors, /*onFocus, onBlur,*/ onPrime, onAutocomplete}) => {
  const [view, setView] = React.useState(null);
  const [to, setTo] = React.useState(null);

  const onChange = React.useCallback((id, str) => {
    onPrime(str);
    setTo(cto => {
      if (cto !== null) clearTimeout(cto);
      if (str.length < 10) return null;
      return setTimeout(onAutocomplete.bind(null, id, str), 150);
    });
  }, [setTo, onPrime, onAutocomplete]);

  React.useEffect(() => {
    if (ref.current !== null) {
      setView(currentView => {
        if (currentView !== null) {
          currentView.destroy();
        }
        const cm = new EditorView({
          state: EditorState.create({
            doc,
            extensions: [
              keymaps,
              keymap.of([indentWithTab]),
              ...extensions,
              sql({
                dialect: PostgreSQL,
                // schema,
              }),
              // autocompletion(),
              shouldUseDarkColors ? xcodeDark : xcodeLight,
              copilot({
                onChange,
              }),
              // EditorView.updateListener.of(update => {
              //   const {view, docChanged, focusChanged, state: {doc}} = update;
              //   if (docChanged) {
              //     const str = doc.toString();
              //     onChange(str);
              //     setTo(cto => {
              //       if (cto !== null) clearTimeout(cto);
              //       if (str.length < 10) return null;
              //       return setTimeout(onAutoComplete.bind(null, str), 150);
              //     })
              //   }
              // }),
            ],
          }),
          parent: ref.current,
        });
        return cm;
      });
    }
  }, [id, ref.current, shouldUseDarkColors, onChange, /*schema,*/ /*onPrime, onAutocomplete, setTo*/]);

  React.useEffect(() => () => {
    setView((view) => {
      if (view) {
        view.destroy();
      }
      return null;
    });
  }, [setView]);

  const onSuggestionAppend = React.useCallback((reqId, token) => {
    // console.log(token);
    view.dispatch({
      effects: SuggestionEffect.of({id: reqId, text: token}),
    });
  }, [view]);

  return [view, onSuggestionAppend];
};
