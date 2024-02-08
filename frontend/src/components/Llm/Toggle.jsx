import React from 'react';
import * as Icons from 'untitledui-js-base';

import store from '@pxl/utils/store.js';
import * as Wails from '@wails/go/app/App';
import * as Runtime from '@wails/runtime';

const LlmToggle = () => {
  const {state: {llm: {enabled, loaded}}, dispatch} = React.useContext(store);

  const onToggle = React.useCallback(() => {
    Wails.Cfg_LlmEnabledSet(!enabled);
    dispatch({
      type: 'LLM-ENABLED',
      enabled: !enabled,
    });
  }, [enabled, dispatch]);

  const Icon = enabled ? Icons.Zap : Icons.ZapOff;

  return (
    <button
      className='p-1 text-gray-400 hover:bg-gray-300 dark:hover:bg-gray-600 hover:rounded-md'
      style={{'--wails-draggable': 'no-drag'}}
      onClick={onToggle}
      title={enabled ? 'Disable LLM autocomplete' : 'Enable LLM autocomplete'}
    >
      <Icon size={20} stroke='currentColor' />
    </button>
  );
}

export default LlmToggle;