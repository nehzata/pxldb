import React from 'react';
import * as Icons from 'untitledui-js-base';

import store from '@pxl/utils/store.js';
import * as Wails from '@wails/go/app/App';

import Table from './Table';


const refresh = async (dsn, force, cb) => {
  cb(null);
  try {
    const schema = await Wails.Dbs_GetSchema(dsn, true);
    cb(schema);
  } catch (err) {
    console.error(err);
    cb(false);
  }
};

const Rightbar = () => {
  const {
    state: {
      dbs: {index: dbIndex, list: dbs},
      misc: {rightbar: visible},
      sessions: {index: sessionIndex, list: sessions},
      tabs: {activeId},
    },
    dispatch,
  } = React.useContext(store);

  const [isVisible, db, session] = React.useMemo(() => {
    if (!visible || activeId === null) return [false, null, null];
    const session = sessions[sessionIndex[activeId]] || null;
    const db = dbs[dbIndex[session.dbId]] || null;
    return [db !== null && session !== null, db, session];
  }, [visible, activeId, dbs, sessions]);

  const onRefresh = React.useCallback(e => {
    refresh(db.dsn, true, schema => {
      dispatch({
        type: 'DBS-SET-SCHEMA',
        id: db.id,
        schema,
      });
    });
  }, [db]);

  React.useEffect(() => {
    if (activeId !== null && db !== null) {
      refresh(db.dsn, false, schema => {
        dispatch({
          type: 'DBS-SET-SCHEMA',
          id: db.id,
          schema,
        });
      });
    }
  }, [db?.dsn]);

  const {name = null, schema = null} = db || {};
  const isRefreshing = schema === null;

  return (
    <div className={isVisible
      ? 'fixed right-0 w-[250px] flex flex-col bg-gray-200 dark:bg-subcanvas-dark border-l border-divider dark:border-divider-dark h-full transition-transform ease-out'
      : 'fixed right-0 w-[250px] flex flex-col bg-gray-200 dark:bg-subcanvas-dark border-l border-divider dark:border-divider-dark h-full transition-transform ease-out translate-x-[250px]'
    }>
      <div
        className='px-2 h-[40px] flex-grow-0 flex-shrink-0 flex flex-row justify-between items-center border border-b border-b-divider dark:border-none'
        style={{'--wails-draggable': 'drag'}}
      >
        <span className='text-sm font-semibold text-gray-800 dark:text-gray-300'>{name}</span>
        <button
          type='button'
          className={isRefreshing
            ? 'p-1 text-gray-400 animate-spin-ccw'
            : 'p-1 text-gray-400 hover:bg-gray-300 dark:hover:bg-gray-600 hover:rounded-md'
          }
          style={{'--wails-draggable': 'no-drag'}}
          onClick={onRefresh}
          disabled={isRefreshing}
        >
          {isRefreshing && (
            <Icons.RefreshCCW03 size={20} stroke='currentColor' />
          )}
          {!isRefreshing && (
            <Icons.RefreshCCW01 size={20} stroke='currentColor' />
          )}
        </button>
      </div>
      {schema === null && (
        <div className='flex-grow flex flex-row justify-center items-center text-gray-400'>
          loading...
        </div>
      )}
      {activeId && schema && schema.tables.length != 0 && (
        <div className='flex-grow flex flex-col py-2 overflow-y-scroll overflow-x-hidden space-y-1 p-2'>
          {schema.tables.map((t, i) => <Table dsn={db.dsn} sId={session.id} {...t} key={i} />)}
        </div>
      )}
    </div>
  );
}

export default Rightbar;