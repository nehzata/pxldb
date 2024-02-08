import React from 'react';
import * as Icons from 'untitledui-js-base';

const TabPill = ({onClick, active, title, disabled, children}) => (
  <button
    type='button'
    className={active
      ? 'first:rounded-bl-md last:rounded-br-md border-r border-gray-300 last:border-none p-1 bg-black text-white'
      : 'first:rounded-bl-md last:rounded-br-md border-r border-gray-300 last:border-none p-1 disabled:text-gray-300'
    }
    onClick={onClick}
    title={title}
    disabled={disabled}
  >{children}</button>
);

const Tabs = ({onSelect, onExport, active, hasError}) => (
  <div className='flex flex-row justify-between items-center border shadow-sm rounded-b-md bg-gray-100'>
    <TabPill onClick={onSelect.bind(null, 'rows')} active={active === 'rows'} title='Query rows' disabled={false}>
      <Icons.Table size={16} stroke='currentColor' />
    </TabPill>
    <TabPill onClick={onSelect.bind(null, 'explain')} active={active === 'explain'} title='EXPLAIN' disabled={hasError}>
      <Icons.File03 size={16} stroke='currentColor' />
    </TabPill>
    <TabPill onClick={onSelect.bind(null, 'analyze')} active={active === 'analyze'} title='EXPLAIN ANALYZE' disabled={hasError}>
      <Icons.Data size={16} stroke='currentColor' />
    </TabPill>
    <button
      type='button'
      className='first:rounded-bl-md last:rounded-br-md border-r border-gray-300 last:border-none p-1 disabled:text-gray-300'
      title='Export rows'
      onClick={onExport}
      disabled={hasError}
    >
      <Icons.Save01 size={16} stroke='currentColor' />
    </button>
  </div>
);

export default Tabs;