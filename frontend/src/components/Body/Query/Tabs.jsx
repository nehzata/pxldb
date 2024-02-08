import React from 'react';
import * as Icons from 'untitledui-js-base';

const TabPill = ({onClick, active, title, children}) => (
  <button
    type='button'
    className={active
      ? 'first:rounded-bl-md last:rounded-br-md border-r border-gray-300 last:border-none p-1 bg-black text-white'
      : 'first:rounded-bl-md last:rounded-br-md border-r border-gray-300 last:border-none p-1'
    }
    onClick={onClick}
    title={title}
  >{children}</button>
);

const Tabs = ({onSelect, active}) => (
  <div className='flex flex-row justify-between items-center border shadow-sm rounded-b-md bg-gray-100'>
    <TabPill onClick={onSelect.bind(null, 'rows')} active={active === 'rows'} title='Query rows'>
      <Icons.Table size={16} stroke='currentColor' />
    </TabPill>
    <TabPill onClick={onSelect.bind(null, 'explain')} active={active === 'explain'} title='EXPLAIN'>
      <Icons.File03 size={16} stroke='currentColor' />
    </TabPill>
    <TabPill onClick={onSelect.bind(null, 'analyze')} active={active === 'analyze'} title='EXPLAIN ANALYZE'>
      <Icons.Data size={16} stroke='currentColor' />
    </TabPill>
  </div>
);

export default Tabs;