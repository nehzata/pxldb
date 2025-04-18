import React from 'react';

import ResultCell from './ResultCell';
import Tabs from './Tabs';


const lengthStr = bytes => {
  const _kb = 1024;
  const _mb = 1024 * _kb;
  const _gb = 1024 * _mb;
  const _tb = 1024 * _gb;

  if (bytes >= _tb) {
    return `${(bytes / _tb).toFixed(1)} TB`;
  } else if (bytes >= _gb) {
    return `${(bytes / _gb).toFixed(1)} GB`;
  } else if (bytes >= _mb) {
    return `${(bytes / _mb).toFixed(1)} MB`;
  } else if (bytes >= _kb) {
    return `${(bytes / _kb).toFixed(1)} KB`;
  } else {
    return `${bytes} bytes`;
  }
};


const truncateString = str => {
  if (typeof str !== 'string' || str.length < 75) {
    return str;
  }

  const len = lengthStr(str.length);
  return [str, `${str.slice(0, 50)}... (${len})`];
};


const ResultRows = ({onChangeView, onExport, id, error = null, numRows = 0, columns = [], rows = [], isStale, isLoading}) => {
  const [isActive, setIsActive] = React.useState(false);
  const [state, setState] = React.useState({id, numRows: Math.min(5, rows.length)});

  const onMouseMove = React.useCallback(e => {
    setIsActive(e.altKey === true || e.metaKey === true);
  }, [isActive, setIsActive]);

  const onMore = React.useCallback(
    e => setState(s => ({...s, numRows: Math.min(rows.length, s.numRows + 20)})),
    [rows, setState],
  );

  React.useEffect(() => {
    setState(s => {
      const {id: oldId, numRows: oldNumRows} = s;
      if (id !== oldId) {
        return {
          id,
          numRows: Math.min(5, rows.length),
        };
      }
      return {
        id,
        numRows: Math.min(oldNumRows < 5 ? 5 : oldNumRows, rows.length),
      };
    });
  }, [id, rows, setState]);

  const displayRows = React.useMemo(() => {
    const {numRows} = state;
    return rows.slice(0, numRows).map(r => r.map(v => truncateString(v)));
  }, [state, rows]);

  return (
    <div>
      <div className={isStale
        ? 'border rounded-l-md rounded-tr-md shadow-sm p-1 overflow-x-scroll text-sm text-gray-900 bg-gray-100'
        : 'border rounded-l-md rounded-tr-md shadow-sm p-1 overflow-x-scroll text-sm text-gray-900'
      }>
        {error !== null && (
          <span>{error}</span>
        )}
        {error === null && (
          <table onMouseMove={onMouseMove} className={isLoading
            ? 'w-full blur-[2px]'
            : 'w-full'
          }>
            {columns.length !== 0 && (
              <thead>
                <tr className='border-b border-divider'>
                  {columns.map((c, i) =>
                    <td className='font-bold px-2 whitespace-nowrap' key={i}>{c}</td>
                  )}
                  <td className='w-full' />
                </tr>
              </thead>
            )}
            <tbody className={isActive ? 'underline hover:cursor-pointer' : ''}>
              {rows.length === 0 && (
                <tr>
                  <td colSpan={columns.length}>
                    ({numRows} rows)
                  </td>
                  <td className='w-full' />
                </tr>
              )}
              {displayRows.map((r, i) =>
                <tr className='border-b border-divider last:border-none' key={i}>
                  {r.map((v, j) => <ResultCell v={v} isStale={isStale} key={j} />)}
                  <td className='w-full' />
                </tr>
              )}
            </tbody>
          </table>
        )}
      </div>
      <div className='flex flex-row justify-between items-center leading-none text-gray-500 text-sm ml-3'>
        <div className='flex flex-row justify-start items-center space-x-2 mt-1'>
          {state.numRows !== rows.length && (
            <button
              type='button'
              className='underline'
              onClick={onMore}
            >more ({rows.length - state.numRows})</button>
          )}
          {isStale && rows.length > 0 && numRows !== undefined && state.numRows != numRows && (
            <div>{numRows} rows</div>
          )}
        </div>
        <Tabs onSelect={onChangeView} onExport={onExport} active={'rows'} hasError={error !== null} />
      </div>
    </div>
  );
};

export default ResultRows;