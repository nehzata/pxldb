import React from 'react';
import * as Icons from 'untitledui-js-base';

import * as QuickCopy from './quick-copy';


const onCopy = (v, e) => {
  if (e.altKey === true) {
    navigator.clipboard.writeText(v);
  } else if (e.metaKey === true) {
    QuickCopy.publish(v);
  }
};

const ResultCell = ({v, isStale}) => {
  let isExpandable = false, vVal = v, vStr = v;
  if (v instanceof Array) {
    isExpandable = true;
    vVal = v[0];
    vStr = v[1];
  }

  const [isExpanded, setIsExpanded] = React.useState(false);

  return (
    <td valign='top'>
      <div className='relative px-2 group'>
        <div
          className='max-w-2xl overflow-scroll whitespace-nowrap'
          onClick={onCopy.bind(null, vVal)}
        >
          {isExpanded ? vVal : vStr}
        </div>
        <div
          className={isStale
            ? 'hidden group-hover:flex flex-row justify-end items-center space-x-2 text-gray-400 absolute right-0 top-0 bottom-0 px-1 bg-gray-100'
            : 'hidden group-hover:flex flex-row justify-end items-center space-x-2 text-gray-400 absolute right-0 top-0 bottom-0 px-1 bg-white'
          }
        >
          <button onClick={() => navigator.clipboard.writeText(vVal)}>
            <Icons.Copy01 size={14} className='transform -scale-x-100' stroke='currentColor' />
          </button>
          {!isExpanded && isExpandable && (
            <button onClick={() => setIsExpanded(true)}>
              <Icons.Expand size={14} stroke='currentColor' />
            </button>
          )}
        </div>
      </div>
    </td>
  );
};

export default ResultCell;