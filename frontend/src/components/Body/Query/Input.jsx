import React from 'react';
import {keymap} from '@codemirror/view';

import store from '@pxl/utils/store';
import * as Icons from 'untitledui-js-base';
import useCodeMirror from '@pxl/components/Codemirror/useCodeMirror';
import * as Wails from '@wails/go/app/App';
import * as Runtime from '@wails/runtime';
import * as QuickCopy from './quick-copy';

const Input = ({dbId, dsn, schema, sId, id, qry}) => {
  const {dispatch} = React.useContext(store);
  const [loading, setLoading] = React.useState(false);

  const execute = React.useCallback(async q => {
    let _id = id;
    try {
      setLoading(true);
      if (_id !== null) {
        dispatch({
          type: 'SESSION-QUERIES-SET-LOADING',
          id: _id,
          sId,
          isLoading: true,
        });
      }
      const _res = await Wails.Dbs_Query(dsn, q);
      const res = {..._res, isPlan: false};
      const resStale = JSON.stringify({
        ...res,
        rows: res.rows === null ? null : res.rows.slice(0, 5),
      });
      if (id === null) {
        _id = await Wails.Cfg_SessionQueriesInsert(sId, q, resStale);
      } else {
        await Wails.Cfg_SessionQueriesUpdate(id, q, resStale);
      }
      await dispatch([
        {
          type: 'SESSION-QUERIES-UPDATE',
          id: _id,
          sId,
          qry: q,
          res,
        },
        {
          type: 'SESSION-QUERIES-SET-LOADING',
          id: _id,
          sId,
          isLoading: false,
        },
      ]);
    } catch (err) {
      console.error(err);
    } finally {
      setLoading(false);
    }
  }, [id, sId]);

  const explain = React.useCallback(async q => {
    let _id = id;
    try {
      setLoading(true);
      if (_id !== null) {
        dispatch({
          type: 'SESSION-QUERIES-SET-LOADING',
          id: _id,
          sId,
          isLoading: true,
        });
      }
      const _res = await Wails.Dbs_Query(dsn, `EXPLAIN ANALYZE ${q}`);
      const res = {..._res, isPlan: true};
      const resStale = JSON.stringify(res);
      if (id === null) {
        _id = await Wails.Cfg_SessionQueriesInsert(sId, q, resStale);
      } else {
        await Wails.Cfg_SessionQueriesUpdate(id, q, resStale);
      }
      await dispatch([
        {
          type: 'SESSION-QUERIES-UPDATE',
          id: _id,
          sId,
          qry: q,
          res,
        },
        {
          type: 'SESSION-QUERIES-SET-LOADING',
          id: _id,
          sId,
          isLoading: false,
        },
      ]);
    } catch (err) {
      console.error(err);
    } finally {
      setLoading(false);
    }
  }, [id, sId]);

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

  // const sqlSchema = React.useMemo(() => {
  //   const {tables = []} = schema || {};
  //   return tables.reduce(
  //     (acc, {name, cols}) => {
  //       const c = cols.reduce((acc, {name}) => [...acc, name], []);
  //       return {
  //         ...acc,
  //         [name]: c,
  //       };
  //     },
  //     {}
  //   );
  // }, [schema]);

  const eventId = React.useMemo(() => [sId, id].join(':'), [sId, id]);

  const onPrime = React.useCallback(q => {
    if (q.trim() === "") return;
    Wails.Llm_Prime(dbId, q);
  }, [dbId]);

  const onAutocomplete = React.useCallback((reqId, q) => {
    if (q.trim() === "") return;
    Wails.Llm_Autocomplete(eventId, reqId, dbId, q);
  }, [dbId, eventId]);

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

  const onExplain = React.useCallback(async e => {
    const q = editor.state.doc.toString();
    explain(q);
  }, [editor]);

  const onDelete = React.useCallback(async e => {
    try {
      await Wails.Cfg_SessionQueriesDelete(id);
      dispatch({
        type: 'SESSION-QUERIES-DELETE',
        id,
        sId,
      });
    } catch (err) {
      console.error(err);
    }
  }, [id, sId]);

  return (
    <>
      <div
        className='border dark:border-divider-dark rounded-md shadow-sm w-full'
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
          >
            <Icons.Trash01 size={16} stroke='currentColor' />
          </button>
        )}
        <button
          type='button'
          className='px-2 py-1 shadow-sm text-white bg-yellow-500 hover:bg-yellow-400 disabled:bg-yellow-300 focus:outline-none rounded-md'
          onClick={onExplain}
          disabled={loading}
        >
          <Icons.PresentationChart01 size={16} stroke='currentColor' />
        </button>
        <button
          type='button'
          className='px-2 py-1 shadow-sm text-white bg-blue-500 hover:bg-blue-400 dark:hover:bg-blue-700 disabled:bg-blue-300 dark:disabled:bg-blue-400 focus:outline-none rounded-md'
          onClick={onExecute}
          disabled={loading}
        >
          <Icons.Play size={16} stroke='currentColor' />
        </button>
      </div>
    </>
  );
};

export default Input;