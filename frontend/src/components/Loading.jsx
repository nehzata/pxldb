import React from 'react';
import byteSize from 'byte-size';
import Logo from '@pxl/assets/logo.svg';

import store from '@pxl/utils/store.js';
import * as Wails from '@wails/go/app/App';
import * as Runtime from '@wails/runtime';

const Loading = () => {
  const {state: {loading: {error, progress, received, total}}, dispatch} = React.useContext(store);

  React.useEffect(() => {
    Runtime.EventsOn('llm-init', data => {
      const {loaded, progress, received, total, error, code} = JSON.parse(data);
      if (error == true) {
        dispatch({
          type: 'LOADING-ERROR',
          code,
        });
      } else if (loaded === true) {
        dispatch({
          type: 'LOADING-DONE',
        });
      } else {
        dispatch({
          type: 'LOADING-UPDATE',
          progress,
          received,
          total,
        });
      }
    });
    return Runtime.EventsOff.bind(null, 'llm-init');
  }, [dispatch]);

  const [receivedStr, totalStr] = React.useMemo(() => {
    if (received === null || total === null) return [null, null];
    return [
      byteSize(received).toString(),
      byteSize(total).toString(),
    ];
  }, [received, total]);

  React.useEffect(() => {
    Wails.Llm_Init();
  }, []);

  return (
    <div className='flex flex-col justify-start items-center h-full text-sm'>
      <div
        className='w-full h-[40px]'
        style={{'--wails-draggable': 'drag'}}
      />
      <div className='flex-grow flex flex-col justify-center items-center'>
        <Logo className='w-1/4 h-1/4 text-gray-100 mb-12 -mt-[40px]' />
        {error !== null && (
          <div className='text-red-500 font-mono'>{error}</div>
        )}
        {error === null && (
          <>
            <div className='font-mono mb-2'>loading model</div>
            <div className='bg-gray-200 rounded-full h-2.5 w-1/4 mb-2'>
              <div className='bg-black h-2.5 rounded-full' style={{width: `${progress}%`}} />
            </div>
            {total !== null && (
              <div className='font-mono'>
                {receivedStr} of {totalStr}
              </div>
            )}
          </>

        )}
      </div>
    </div>
  );
}

export default Loading;