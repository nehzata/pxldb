import React from 'react';
import * as Icons from 'untitledui-js-base';

import store from '@pxl/utils/store.js';

import Empty from './Empty';
import Session from './Session';
import Tab from './Tab';


const Body = () => {
  const {
    state: {
      misc: {
        leftbar: leftbarVisible,
        rightbar: _rightbarVisible,
      },
      tabs: {activeId, tabs},
    },
    dispatch,
  } = React.useContext(store);

  const onToggleLeftbar = React.useCallback(() => {
    dispatch({
      type: 'MISC-LEFTBAR-TOGGLE',
    });
  }, [dispatch]);

  const onToggleRightbar = React.useCallback(() => {
    dispatch({
      type: 'MISC-RIGHTBAR-TOGGLE',
    });
  }, [dispatch]);

  const rightbarVisible = _rightbarVisible && activeId !== null;

  return (
    <div className={leftbarVisible && rightbarVisible
      ? 'relative flex flex-col h-full transition-spacing ease-out mx-[250px]'
      : leftbarVisible
        ? 'relative flex flex-col h-full transition-spacing ease-out ml-[250px]'
        : rightbarVisible ? 'relative flex flex-col h-full transition-spacing ease-out mr-[250px]'
          : 'relative flex flex-col h-full transition-spacing ease-out ml-0'
    }>
      {(!leftbarVisible || activeId !== null) && (
        <div
          className='h-[40px] flex-grow-0 flex-shrink-0 flex flex-row items-center dark:bg-subcanvas-dark border-b border-divider dark:border-divider-dark'
          style={{'--wails-draggable': 'drag'}}
        >
          {!leftbarVisible && (
            <button
              className='ml-28 p-1 text-gray-400 hover:bg-gray-200 dark:hover:bg-gray-600 hover:rounded-md'
              style={{'--wails-draggable': 'no-drag'}}
              onClick={onToggleLeftbar}
            >
              <Icons.LayoutLeft size={20} stroke='currentColor' />
            </button>
          )}
          <div
            className={leftbarVisible
              ? 'flex flex-row overflow-x-scroll'
              : activeId === null
                ? 'flex flex-row overflow-x-scroll ml-6'
                : 'flex flex-row overflow-x-scroll ml-6 border-l'
            }
            style={{'--wails-draggable': 'no-drag'}}
          >
            {tabs.map((t, i) => <Tab {...t} activeId={activeId} key={i} />)}
          </div>
          <div className='flex-grow' />
          {tabs.length !== 0 && (
            <button
              className='ml-6 mr-2 p-1 text-gray-400 hover:bg-gray-200 dark:hover:bg-gray-600 hover:rounded-md'
              style={{'--wails-draggable': 'no-drag'}}
              onClick={onToggleRightbar}
            >
              <Icons.LayoutRight size={20} stroke='currentColor' />
            </button>
          )}
        </div>
      )}
      {activeId === null && (
        <Empty />
      )}
      {activeId !== null && (
        <Session />
      )}
    </div >
  );
}

export default Body;