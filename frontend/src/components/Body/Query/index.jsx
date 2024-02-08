import React from 'react';
import {keymap} from '@codemirror/view';
import * as Icons from 'untitledui-js-base';

import store from '@pxl/utils/store';
import useCodeMirror from '@pxl/components/Codemirror/useCodeMirror';
import * as Wails from '@wails/go/app/App';
import * as Runtime from '@wails/runtime';
import * as QuickCopy from './quick-copy';

import ResultRows from './ResultRows';
import ResultPlan from './ResultPlan';

const Query = ({
  db,
  session,
  // dbId,
  // dsn,
  // schema,
  // sId,
  id,
  qry = '',
  res = null,
  explain = null,
  analyze = null,
  isResStale,
  isExplainStale,
  isAnalyzeStale,
  isLoading,
}) => {
  const {dispatch} = React.useContext(store);
  const [loading, setLoading] = React.useState(false);
  const [view, setView] = React.useState('rows');
  const [error, setError] = React.useState(false);

  const execute = React.useCallback(async q => {
    let _id = id;
    try {
      setLoading(true);
      if (_id !== null) {
        dispatch({
          type: 'SESSION-QUERIES-SET-LOADING',
          id: _id,
          sId: session.id,
          isLoading: true,
        });
      }
      const res = await Wails.Dbs_Query(db.dsn, q);
      const resStale = JSON.stringify({
        ...res,
        rows: res.rows === null ? null : res.rows.slice(0, 5),
      });
      if (id === null) {
        _id = await Wails.Cfg_SessionQueriesInsert(session.id, q, resStale);
      } else {
        await Wails.Cfg_SessionQueriesUpdateRes(id, q, resStale);
      }
      await dispatch([
        {
          type: 'SESSION-QUERIES-UPDATE',
          id: _id,
          sId: session.id,
          qry: q,
          res,
        },
        {
          type: 'SESSION-QUERIES-SET-LOADING',
          id: _id,
          sId: session.id,
          isLoading: false,
        },
      ]);
    } catch (err) {
      console.error(err);
    } finally {
      setLoading(false);
    }
  }, [id, session.id]);

  const explainAnalyze = React.useCallback(async (mode, qry) => {
    if (id === null) return;
    let _id = id;
    try {
      setLoading(true);
      if (_id !== null) {
        dispatch({
          type: 'SESSION-QUERIES-SET-LOADING',
          id: _id,
          sId: session.id,
          isLoading: true,
        });
      }
      const q = [
        'EXPLAIN',
        mode === 'analyze' ? 'ANALYZE' : undefined,
        qry
      ]
        .filter(f => f !== undefined)
        .join(' ');
      const res = await Wails.Dbs_Query(db.dsn, q);
      const resStale = JSON.stringify({
        ...res,
        rows: res.rows === null ? null : res.rows.slice(0, 5),
      });
      if (mode === 'explain') {
        await Wails.Cfg_SessionQueriesUpdateExplain(id, resStale);
      } else if (mode === 'analyze') {
        await Wails.Cfg_SessionQueriesUpdateAnalyze(id, resStale);
      }
      await dispatch([
        {
          type: 'SESSION-QUERIES-UPDATE',
          id: _id,
          sId: session.id,
          qry,
          [mode]: res,
        },
        {
          type: 'SESSION-QUERIES-SET-LOADING',
          id: _id,
          sId: session.id,
          isLoading: false,
        },
      ]);
    } catch (err) {
      console.error(err);
    } finally {
      setLoading(false);
    }
  }, [id, session.id]);

  const keymaps = React.useMemo(() => keymap.of([
    {
      key: 'Cmd-Enter',
      run: view => {
        const q = view.state.doc.toString();
        execute(q);
        return true;
      },
    },
    {
      key: 'Cmd-e',
      run: view => {
        const q = view.state.doc.toString();
        execute(q);
        return true;
      },
    },
  ]), [execute]);

  const eventId = React.useMemo(() => [session.id, id].join(':'), [session.id, id]);

  const onPrime = React.useCallback(q => {
    if (q.trim() === "") {
      setError(false);
      return;
    }
    (async () => {
      try {
        await Wails.Llm_Prime(db.id, q);
        setError(false);
      } catch (err) {
        setError(true);
      }
    })();
  }, [db.id, setError]);

  const onAutocomplete = React.useCallback((reqId, q) => {
    if (q.trim() === "") return;
    Wails.Llm_Autocomplete(eventId, reqId, db.id, q);
  }, [db.id, eventId]);

  const ref = React.useRef();
  const [editor, onSuggestionAppend] = useCodeMirror({
    id,
    doc: qry,
    keymaps,
    ref,
    shouldUseDarkColors: false,
    // dbId,
    // schema: sqlSchema,
    onPrime,
    onAutocomplete,
  });

  React.useEffect(() => {
    Runtime.EventsOn(eventId, data => {
      const {reqId, error, stop, cancel, t} = data;
      if (error !== true && cancel !== true && stop !== true) {
        onSuggestionAppend(reqId, t);
      }
    });
    return Runtime.EventsOff.bind(null, eventId);
  }, [eventId, onSuggestionAppend]);

  React.useEffect(() => {
    return () => {
      QuickCopy.unsubscribe(quickCopy);
    };
  }, []);

  const quickCopy = React.useCallback((v) => {
    if (ref.current) {
      editor.dispatch(editor.state.replaceSelection(v));
      editor.focus();
    }
  }, [ref.current, editor]);

  const onFocus = React.useCallback(() => {
    QuickCopy.subscribe(quickCopy);
  }, [quickCopy]);

  const onBlur = React.useCallback(() => {
    setTimeout(() => {
      QuickCopy.unsubscribe.bind(null, quickCopy);
    }, 50);
  }, [quickCopy]);

  const onExecute = React.useCallback(async e => {
    const q = editor.state.doc.toString();
    execute(q);
  }, [editor]);

  const onExplainAnalyze = React.useCallback(async (mode, e) => {
    console.log('onExplainAnalyze');
    // const q = editor.state.doc.toString();
    explainAnalyze(mode, qry);
  }, [qry, editor]);

  const onExport = React.useCallback(async e => {
    const fn = [db.name, session.title || 'untitled', Math.round(Date.now() / 1000)].join('-');
    await Wails.Utils_ExportRows(`${fn}.csv`, [res.columns, ...res.rows]);
    // console.log(db.name, session.title || 'untitled', res);
  }, [db.name, session.title, res]);

  const onDelete = React.useCallback(async e => {
    try {
      await Wails.Cfg_SessionQueriesDelete(id);
      dispatch({
        type: 'SESSION-QUERIES-DELETE',
        id,
        sId: session.id,
      });
    } catch (err) {
      console.error(err);
    }
  }, [id, session.id]);

  return (
    <div className='flex flex-col space-y-2'>
      <div
        className={error === true
          ? 'border rounded-md shadow-sm w-full cm-error'
          : 'border rounded-md shadow-sm w-full'
        }
        ref={ref}
        onFocus={onFocus}
        onBlur={onBlur}
      />
      <div className='flex flex-row justify-end items-center space-x-1'>
        {id !== null && (
          <button
            type='button'
            className='px-2 py-1 shadow-sm text-white bg-red-500 hover:bg-red-400 focus:outline-none rounded-md'
            onClick={onDelete}
            title='Delete query'
          >
            <Icons.Trash01 size={16} stroke='currentColor' />
          </button>
        )}
        <button
          type='button'
          className='px-2 py-1 shadow-sm text-white bg-blue-500 hover:bg-blue-400 dark:hover:bg-blue-700 disabled:bg-blue-300 dark:disabled:bg-blue-400 focus:outline-none rounded-md'
          onClick={onExecute}
          disabled={loading}
          title='Execute query [⌘+Enter]'
        >
          <Icons.Play size={16} stroke='currentColor' />
        </button>
      </div>
      {view === 'rows' && res !== null && (
        <ResultRows
          onChangeView={setView.bind(null)}
          onExport={res.error === null ? onExport : null}
          id={id}
          isStale={isResStale}
          isLoading={isLoading}
          {...res}
        />
      )}
      {view === 'explain' && (
        <ResultPlan
          mode={view}
          onChangeView={setView.bind(null)}
          onLoad={onExplainAnalyze.bind(null, view)}
          onExport={onExport}
          query={qry}
          isStale={isExplainStale}
          isLoading={isLoading}
          hasError={res !== null && res.error !== null}
          {...explain}
        />
      )}
      {view === 'analyze' && (
        <ResultPlan
          mode={view}
          onChangeView={setView.bind(null)}
          onLoad={onExplainAnalyze.bind(null, view)}
          onExport={onExport}
          query={qry}
          isStale={isAnalyzeStale}
          isLoading={isLoading}
          hasError={res !== null && res.error !== null}
          {...analyze}
        />
      )}
    </div>
  );
};

export default Query;