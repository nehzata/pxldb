import React from 'react';

import ConfirmModal from '@pxl/atoms/ConfirmModal';
import {Trash} from '@pxl/atoms/Icons';
import store from '@pxl/utils/store.js';
import * as Wails from '@wails/go/app/App';

const Session = ({id, title}) => {
  const {state: {tabs: {activeId}}, dispatch} = React.useContext(store);

  const onClick = React.useCallback(
    e => {
      dispatch({type: 'TABS-OPEN', id});
      // if (e.metaKey === true) {
      //   const {pxl} = window;
      //   pxl.createWindowWithSession(id);
      // } else {
      //   dispatch({type: 'TABS-OPEN', id});
      // }
    },
    [dispatch, id],
  );

  const onDelete = React.useCallback(
    e => {
      Wails.Cfg_SessionsDelete(id);
      dispatch([
        {type: 'TABS-CLOSE', id},
        {type: 'SESSIONS-DELETE', id},
      ]);
    },
    [dispatch, id],
  );

  const isActive = React.useMemo(
    () => (activeId === id),
    [activeId, id],
  );

  return (
    <div className='ml-12 group flex flex-row justify-between items-center space-x-2 h-6'>
      <button
        type='button'
        className={isActive
          ? 'flex-grow overflow-hidden text-ellipsis focus:outline-none text-black dark:text-gray-200 text-left text-sm font-semibold'
          : 'flex-grow overflow-hidden text-ellipsis focus:outline-none text-black dark:text-gray-200 text-left text-sm'
        }
        onClick={onClick}
      >
        {title || 'untitled'}
      </button >
      <ConfirmModal
        btn={
          <Trash width={16} height={16} />
        }
        btnClassName='hidden group-hover:block flex-shrink-0 text-red-500 hover:bg-gray-300 p-1 rounded'
        title='Delete session?'
        text='Are you sure you wish to delete this session?'
        onConfirm={onDelete}
      />
    </div >
  );
}

export default Session;