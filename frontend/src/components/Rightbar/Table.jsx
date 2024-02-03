import React from 'react';
import * as Icons from 'untitledui-js-base';

import store from '@pxl/utils/store';
import * as Wails from '@wails/go/app/App';

const templates = {
  select: table => [true, `SELECT *\nFROM ${table}\nLIMIT 100;`],
  insert: table => [false, `INSERT INTO ${table} ()\nVALUES ();`],
  update: table => [false, `UPDATE ${table}\nSET\n WHERE\n;`],
  delete: table => [false, `DELETE FROM ${table}\nWHERE\n;`],
};

const Table = ({dsn, sId, name, cols}) => {
  const {dispatch} = React.useContext(store);
  const [isExpanded, setIsExpanded] = React.useState(false);

  const onToggle = React.useCallback(() => {
    setIsExpanded(e => !e);
  }, [setIsExpanded]);

  const onTemplate = React.useCallback(async (e) => {
    const tmpl = e.currentTarget.getAttribute('data-template');
    const [shouldExecute, qry] = templates[tmpl](name);

    const _res = shouldExecute === true
      ? await Wails.Dbs_Query(dsn, qry)
      : null;
    const res = _res === null
      ? null
      : JSON.stringify({..._res, rows: _res.rows.slice(0, 5)});
    const id = await Wails.Cfg_SessionQueriesInsert(sId, qry, res);
    dispatch({
      type: 'SESSION-QUERIES-UPDATE',
      id,
      sId,
      qry,
      res: _res,
    });

  }, [name]);

  // const onInsert = React.useCallback(async () => {
  //   const {pxl} = window;
  //   const qry = insertTemplate(name);
  //   const id = await pxl.cfg('session_queries_insert', sId, qry, null);
  //   pxl.dispatch({
  //     type: 'SESSION-QUERIES-UPDATE',
  //     id,
  //     sId,
  //     qry,
  //     res: null,
  //   });
  // }, [name]);

  return (
    <div className='flex flex-col space-y-1'>
      <div className='group flex flex-row justify-start items-center h-6'>
        <button
          className='flex-grow flex flex-row justify-start items-center space-x-2 overflow-hidden focus:outline-none mr-2'
          onClick={onToggle}
        >
          {isExpanded && <Icons.ChevronDown className='flex-shrink-0 text-gray-500' size={16} stroke='currentColor' />}
          {!isExpanded && <Icons.ChevronRight className='flex-shrink-0 text-gray-500' size={16} stroke='currentColor' />}
          <Icons.Table className='flex-shrink-0 text-gray-500' size={16} stroke='currentColor' />
          <div className='overflow-hidden whitespace-nowrap text-ellipsis text-gray-800 dark:text-gray-300 text-sm'>{name}</div>
        </button>
        <button
          className='hidden group-hover:block p-1 text-gray-400 hover:bg-gray-300 dark:hover:bg-gray-600 hover:rounded-md focus:outline-none'
          onClick={onTemplate}
          data-template='select'
        >
          <Icons.SearchSM className='flex-shrink-0' size={16} stroke='currentColor' />
        </button>
        <button
          className='hidden group-hover:block p-1 text-gray-400 hover:bg-gray-300 dark:hover:bg-gray-600 hover:rounded-md focus:outline-none'
          onClick={onTemplate}
          data-template='insert'
        >
          <Icons.Plus className='flex-shrink-0' size={16} stroke='currentColor' />
        </button>
        <button
          className='hidden group-hover:block p-1 text-gray-400 hover:bg-gray-300 dark:hover:bg-gray-600 hover:rounded-md focus:outline-none'
          onClick={onTemplate}
          data-template='delete'
        >
          <Icons.Minus className='flex-shrink-0' size={16} stroke='currentColor' />
        </button>
        <button
          className='hidden group-hover:block p-1 text-gray-400 hover:bg-gray-300 dark:hover:bg-gray-600 hover:rounded-md focus:outline-none'
          onClick={onTemplate}
          data-template='update'
        >
          <Icons.Pencil01 className='flex-shrink-0' size={16} stroke='currentColor' />
        </button>
      </div>
      {isExpanded && cols.map((col, i) => (
        <div className='ml-12 text-gray-800 dark:text-gray-300 text-sm overflow-hidden whitespace-nowrap text-ellipsis' key={i}>{col.shortDesc}</div>
      ))}
    </div>
  );
}

export default Table;