import React from 'react';
import * as Icons from 'untitledui-js-base';

import EditDB from '@pxl/components/EditDB';
import store from '@pxl/utils/store.js';

import DB from './DB';


const Leftbar = () => {
  const [open, setOpen] = React.useState(false);
  const {state: {dbs: {list: dbs}, misc: {leftbar: visible}}, dispatch} = React.useContext(store);

  const onToggle = React.useCallback(() => {
    dispatch({
      type: 'MISC-LEFTBAR-TOGGLE',
    });
  }, [dispatch]);

  return (
    <div className={visible
      ? 'fixed w-[250px] flex flex-col bg-gray-200 dark:bg-subcanvas-dark border-r border-divider dark:border-divider-dark h-full transition-transform ease-out'
      : 'fixed w-[250px] flex flex-col bg-gray-200 dark:bg-subcanvas-dark border-r border-divider dark:border-divider-dark h-full transition-transform ease-out translate-x-[-250px]'
    }
    >
      <div
        className='h-[40px] flex-grow-0 flex-shrink-0 flex flex-row justify-end items-center pr-2'
        style={{'--wails-draggable': 'drag'}}
      >
        <button
          className='p-1 text-gray-400 hover:bg-gray-300 dark:hover:bg-gray-600 hover:rounded-md'
          style={{'--wails-draggable': 'no-drag'}}
          onClick={onToggle}
          disabled={false}
        >
          <Icons.LayoutLeft size={20} stroke='currentColor' />
        </button>
      </div>
      {dbs.length === 0 && (
        <div className='flex flex-col justify-center items-center h-full space-y-2'>
          <button
            className='p-1 text-gray-400 hover:bg-gray-300 dark:hover:bg-gray-600 hover:rounded-md mt-[-40px]'
            onClick={setOpen.bind(null, true)}
          >
            <Icons.Plus size={20} stroke='currentColor' />
          </button>
          <div className='text-sm text-gray-400'>add db</div>
        </div>
      )}
      {dbs.length !== 0 && (
        <div className='flex-grow flex flex-col pl-3 pr-2 py-2 overflow-scroll space-y-4'>
          {dbs.map((db, i) => (
            <DB {...db} key={i} />
          ))}
        </div>
      )}
      {dbs.length > 0 && (
        <button
          type='button'
          className='text-gray-500 dark:text-gray-200 bg-gray-300 dark:bg-gray-500 hover:bg-opacity-70 text-center border dark:border-control-dark rounded-md m-2 p-2'
          onClick={setOpen.bind(null, true)}
        >
          <Icons.Plus size={20} className='mx-auto' stroke='currentColor' />
        </button>
      )}
      <EditDB id={null} dsn={''} name={''} open={open} onClose={setOpen.bind(null, false)} />
    </div>
  );
}

export default Leftbar;