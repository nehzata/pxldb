import React from 'react';

import * as Icons from '@pxl/atoms/Icons';
import store from '@pxl/utils/store.js';
import EditDB from '@pxl/components/EditDB';
import * as Wails from '@wails/go/app/App';

import Session from './Session';


const DB = ({id, dsn, name, isExpanded}) => {
  const {state: {sessions: {list: allSessions}}, dispatch} = React.useContext(store);
  const [settingsOpen, setSettingsOpen] = React.useState(false);

  const sessions = React.useMemo(
    () => allSessions.filter(s => s.dbId === id),
    [allSessions],
  );

  const onToggle = () => {
    Wails.Cfg_DbsSetIsExpanded(id, !isExpanded);
    dispatch({type: 'DBS-SET-IS-EXPANDED', id, isExpanded: !isExpanded});
  };

  const onAdd = () => {
    (async () => {
      const sId = await Wails.Cfg_SessionsInsert(id);
      dispatch([
        {type: 'SESSIONS-ADD', id: sId, dbId: id},
        {
          type: 'TABS-OPEN',
          id: sId,
          isPreview: false,
        },
      ]);
    })();
  };

  return (
    <div className='flex flex-col space-y-1'>
      <div className='group flex flex-row justify-start items-center text-gray-400 dark:text-gray-400 h-6'>
        <button
          className='flex-grow flex flex-row justify-start items-center space-x-2 overflow-hidden focus:outline-none mr-2'
          onClick={onToggle}
        >
          {isExpanded && <Icons.ChevronDown className='flex-shrink-0' width={16} height={16} />}
          {!isExpanded && <Icons.ChevronRight className='flex-shrink-0' width={16} height={16} />}
          <Icons.Database className='flex-shrink-0' width={16} height={16} />
          <div className='overflow-hidden whitespace-nowrap text-ellipsis text-sm'>{name}</div>
        </button>
        <button
          className='p-1 hover:bg-gray-300 dark:hover:bg-gray-600 hover:rounded-md focus:outline-none'
          onClick={onAdd}
        >
          <Icons.Plus className='hidden group-hover:block flex-shrink-0' width={16} height={16} />
        </button>
        <button
          className='p-1 hover:bg-gray-300 dark:hover:bg-gray-600 hover:rounded-md focus:outline-none'
          onClick={() => setSettingsOpen(true)}
        >
          <Icons.Settings className='hidden group-hover:block flex-shrink-0' width={16} height={16} />
        </button>
        <EditDB
          id={id}
          dsn={dsn}
          name={name}
          open={settingsOpen}
          onClose={() => setSettingsOpen(false)}
        />
      </div>
      {isExpanded && sessions.map((session, i) => (
        <Session dbId={id} {...session} key={i} />
      ))}
    </div>
  );
}

export default DB;