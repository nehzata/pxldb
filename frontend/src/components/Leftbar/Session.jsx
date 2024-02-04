import React from 'react';
import * as Icons from 'untitledui-js-base';

import ConfirmModal from '@pxl/atoms/ConfirmModal';
import store from '@pxl/utils/store.js';
import * as Wails from '@wails/go/app/App';

const Session = ({id, title}) => {
  const {state: {tabs: {activeId}}, dispatch} = React.useContext(store);
  const [value, setValue] = React.useState(title);
  const [editing, setEditing] = React.useState(false);

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

  const onEdit = React.useCallback(e => {
    setValue(e.target.value);
  }, [setValue])

  const onSave = React.useCallback(e => {
    Wails.Cfg_SessionsUpdate(id, value)
    setEditing(false);
  }, [id, value, setEditing]);

  const isActive = React.useMemo(
    () => (activeId === id),
    [activeId, id],
  );

  if (editing === true) {
    return (
      <div className='-ml-1 flex flex-row justify-between items-center h-6'>
        <div className='flex-grow flex-shrink'>
          <input
            type='text'
            className='border rounded-l-md w-full focus:outline-none px-1'
            value={value || ''}
            onChange={onEdit}
            onBlur={() => setEditing(false)}
            autoFocus={true}
          />
        </div>
        <button
          type='button'
          className='flex justify-center items-center rounded-r-md bg-black text-white h-6 w-6 flex-shrink-0'
          onClick={onSave}
        >
          <Icons.Check size={14} stroke='currentColor' />
        </button>
      </div>
    );
  }

  return (
    <div className='group flex flex-row justify-between items-center space-x-2 h-6'>
      <button
        type='button'
        className={isActive
          ? 'flex-grow overflow-hidden text-ellipsis focus:outline-none text-black dark:text-gray-200 text-left text-sm font-semibold'
          : 'flex-grow overflow-hidden text-ellipsis focus:outline-none text-black dark:text-gray-200 text-left text-sm'
        }
        onClick={onClick}
        onDoubleClick={() => setEditing(true)}
      >
        {value || 'untitled'}
      </button >
      <ConfirmModal
        btn={
          <Icons.Trash01 size={16} stroke='currentColor' />
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