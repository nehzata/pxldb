import React from 'react';

const TabPill = ({onClick, active, children}) => (
  <button
    type='button'
    className={active
      ? 'first:rounded-bl-md last:rounded-br-md border-r border-gray-300 last:border-none py-1 w-20 bg-gray-800 text-white'
      : 'first:rounded-bl-md last:rounded-br-md border-r border-gray-300 last:border-none py-1 w-20'
    }
    onClick={onClick}
  >{children}</button>
);

const Tabs = ({onSelect, active}) => (
  <div className='flex flex-row justify-between items-center border shadow-sm rounded-b-md bg-gray-100'>
    <TabPill onClick={onSelect.bind(null, 'rows')} active={active === 'rows'}>rows</TabPill>
    <TabPill onClick={onSelect.bind(null, 'explain')} active={active === 'explain'}>explain</TabPill>
    <TabPill onClick={onSelect.bind(null, 'analyze')} active={active === 'analyze'}>analyze</TabPill>
  </div>
);

export default Tabs;