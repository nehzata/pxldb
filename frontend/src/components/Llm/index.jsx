import React from 'react';
import byteSize from 'byte-size';

import store from '@pxl/utils/store.js';
import Logo from '@pxl/assets/logo.svg';
import * as Wails from '@wails/go/app/App';
import * as Runtime from '@wails/runtime';

const Llm = ({children}) => {
  const {state: {llm: {enabled, loaded, error, progress, received, total}}, dispatch} = React.useContext(store);

  React.useEffect(() => {
    (async () => {
      const [enabled, loaded] = await Promise.all([
        Wails.Cfg_LlmEnabledGet(),
        Wails.Llm_StatusGet(),
      ]);
      dispatch([
        {
          type: 'LLM-ENABLED',
          enabled,
        },
        {
          type: 'LLM-LOADED',
          loaded,
        },
      ]);
    })();
  }, []);

  React.useEffect(() => {
    if (enabled === true && loaded === false) {
      Wails.Llm_Init();
    } else if (enabled === false && loaded === true) {
      Wails.Llm_Close();
    }
  }, [enabled, loaded]);

  React.useEffect(() => {
    Runtime.EventsOn('llm-status', data => {
      const {loaded, progress, received, total, error, code} = JSON.parse(data);
      if (error == true) {
        dispatch({
          type: 'LLM-ERROR',
          code,
        });
      } else if (loaded === true || loaded === false) {
        dispatch({
          type: 'LLM-LOADED',
          loaded,
        });
      } else {
        dispatch({
          type: 'LLM-UPDATE',
          progress,
          received,
          total,
        });
      }
    });
    return Runtime.EventsOff.bind(null, 'llm-status');
  }, [dispatch]);

  const [receivedStr, totalStr] = React.useMemo(() => {
    if (enabled !== true || received === null || total === null) return [null, null];
    return [
      byteSize(received).toString(),
      byteSize(total).toString(),
    ];
  }, [received, total]);

  if (enabled && loaded !== true) {
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

  return children;
}

export default Llm;