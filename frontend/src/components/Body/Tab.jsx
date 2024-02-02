import React from 'react';

import {X} from '@pxl/atoms/Icons';
import store from '@pxl/utils/store.js';

const Tab = ({id, isPreview, activeId}) => {
  const {state: {sessions: {index: sessionsIndex, list: sessions}}, dispatch} = React.useContext(store);

  const isActive = React.useMemo(() => id === activeId, [id, activeId]);
  const title = React.useMemo(() => {
    const idx = sessionsIndex[id];
    return sessions[idx].title;
  }, [id, sessions]);

  const onSelect = React.useCallback(e => dispatch({
    type: 'TABS-OPEN',
    id,
    isPreview: false,
  }), [id, dispatch]);

  const onClose = React.useCallback((e) => {
    dispatch({
      type: 'TABS-CLOSE',
      id,
    });
    e.stopPropagation();
    e.preventDefault();
  }, [id, dispatch]);

  return (
    <div
      className={isActive
        ? 'group flex-grow-0 flex-shrink-0 flex flex-row justify-between items-center h-[40px] w-24 space-x-1 pl-2 pr-1 cursor-pointer dark:bg-canvas-dark dark:border-divider-dark dark:border-t-gray-300 border-r border-b border-b-canvas dark:border-b-canvas-dark border-t border-t-black border-t-[2px] '
        : 'group flex-grow-0 flex-shrink-0 flex flex-row justify-between items-center h-[40px] w-24 space-x-1 pl-2 pr-1 cursor-pointer dark:border-divider-dark border-r border-b border-t-canvas dark:border-t-subcanvas-dark border-t-[2px]'
      }
      onClick={onSelect}
    >
      <span className={!isPreview
        ? isActive
          ? 'text-sm dark:text-white overflow-hidden text-ellipsis whitespace-nowrap'
          : 'text-sm dark:text-zinc-400 overflow-hidden text-ellipsis whitespace-nowrap'
        : isActive
          ? 'text-sm dark:text-white overflow-hidden text-ellipsis whitespace-nowrap italic'
          : 'text-sm dark:text-zinc-400 overflow-hidden text-ellipsis whitespace-nowrap italic'
      }>{title || 'untitled'}</span>
      <div
        className={isActive
          ? 'p-1 text-gray-400 dark:text-white hover:bg-gray-200 dark:hover:bg-gray-700 rounded-md'
          : 'p-1 text-gray-400 dark:text-gray-400 hover:bg-gray-200 dark:hover:bg-gray-700 rounded-md hidden group-hover:block'
        }
        onClick={onClose}
      >
        <X width={16} height={16} />
      </div>
    </div >
  );
}

export default Tab;