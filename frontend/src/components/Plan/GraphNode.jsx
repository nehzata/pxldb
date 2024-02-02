import React from 'react';

import * as C from './consts';
import * as utils from './utils';
import * as Icons from '@pxl/atoms/Icons';

const GraphNode = ({plan, node, onSize}) => {
  const ref = React.useRef();

  React.useEffect(() => {
    const size = ref.current.getBoundingClientRect();
    onSize(size.width, size.height);

  }, []);

  const {
    nodeName,
    durationClass,
    costClass,
    estimationClass,
    rowsRemovedClass,
    workersLaunchedCount,
    workersPlannedCount,
    workersPlannedCountReversed,
  } = React.useMemo(() => {
    const executionTime = Number(plan.planStats.executionTime) ||
      Number(plan.content?.Plan?.[C.ACTUAL_TOTAL_TIME]);
    const duration = Number(node[C.EXCLUSIVE_DURATION]);
    const executionTimePercent = Math.round((duration / executionTime) * 100);

    const maxTotalCost = Number(plan.content.maxTotalCost);
    const cost = Number(node[C.EXCLUSIVE_COST]);
    const costPercent = Math.round((cost / maxTotalCost) * 100);

    const rowsRemoved = Object.entries(node).reduce((acc, [key, value]) => {
      if (acc === null && (
        key === C.ROWS_REMOVED_BY_FILTER_REVISED ||
        key === C.ROWS_REMOVED_BY_JOIN_FILTER_REVISED
      )) {
        return value;
      }
      return acc;
    }, null);

    let rowsRemovedPercentage = null;
    if (rowsRemoved) {
      const actual = Number(node[C.ACTUAL_ROWS_REVISED]);
      rowsRemovedPercentage = Math.floor(rowsRemoved / (rowsRemoved + actual) * 100);
    }

    const severityClasses = [
      'p-1 rounded bg-red-400',
      'p-1 rounded bg-orange-400',
      'p-1 rounded bg-yellow-400',
      'p-1 rounded bg-gray-400',
    ];
    const durationClass = [90, 40, 10].reduce((acc, limit, i) => {
      if (acc === null && executionTimePercent > limit) return severityClasses[i];
      return acc;
    }, null);
    const costClass = [90, 40, 10].reduce((acc, limit, i) => {
      if (acc === null && costPercent > limit) return severityClasses[i];
      return acc;
    }, null);
    const estimationClass = [1000, 100, 10].reduce((acc, limit, i) => {
      if (acc === null && Number(node[C.PLANNER_ESTIMATE_FACTOR]) > limit) return severityClasses[i];
      return acc;
    }, null);
    const rowsRemovedClass = [2000, 500].reduce((acc, limit, i) => {
      if (acc === null && rowsRemovedPercentage !== null && rowsRemovedPercentage * executionTimePercent > limit) return severityClasses[i];
      return acc;
    }, null);


    const workersLaunchedCount = (() => {
      console.warn("Make sure it works for workers that are not array")
      if (node[C.WORKERS_LAUNCHED]) {
        return Number(node[C.WORKERS_LAUNCHED]);
      }
      const workers = node[C.WORKERS];
      return workers ? workers.length : NaN;
    })();
    const workersPlannedCount = node[C.WORKERS_PLANNED_BY_GATHER];
    const workersPlannedCountReversed = [...Array(workersPlannedCount).keys()]
      .slice()
      .map(v => v + 1)
      .reverse();

    return {
      nodeName: node[C.PARALLEL_AWARE]
        ? `Parallel ${node[C.NODE_TYPE]}`
        : node[C.NODE_TYPE],
      executionTime,
      executionTimePercent,
      durationClass,
      costClass,
      estimationClass,
      rowsRemovedClass,
      workersLaunchedCount,
      workersPlannedCount,
      workersPlannedCountReversed,
    };
  }, [node]);

  return (
    <div className='text-xs w-[240px] mx-auto' ref={ref}>
      <div className='text-center w-[240px]'>
        {node[C.SUBPLAN_NAME] && (
          <div className='flex justify-center items-center'>
            <div className='font-bold italic px-1 whitespace-nowrap text-xs bg-blue-200 rounded'>
              {node[C.SUBPLAN_NAME]}
            </div>
          </div>
        )}
        {false && workersPlannedCount > 0 && (
          <div className='absolute w-full h-full'>
            {workersPlannedCountReversed.map((idx, i) => (
              <div
                className='absolute w-full h-full border bg-white border rounded'
                style={{
                  top: `${3 * idx}px`,
                  left: `${3 * idx}px`,
                }}
                key={i}
              >{i}</div>
            ))}
          </div>
        )}
        {/*workers div*/}
        <div className='bg-white border border-gray-300 rounded shadow p-1'>
          <div className='flex flex-row justify-between items-center whitespace-nowrap'>
            <div className='flex flex-row justify-start items-center space-x-1 overflow-hidden'>
              <Icons.ChevronUp width={20} height={20} className='flex-shrink-0 flex-grow-0 text-gray-400' />
              <div className='text-xs font-semibold overflow-hidden text-ellipsis'>{nodeName}</div>
            </div>
            <div className='flex flex-row justify-end items-center space-x-1'>
              {durationClass !== null && (
                <div className={durationClass}>
                  <Icons.Clock width={12} height={12} />
                </div>
              )}
              {costClass !== null && (
                <div className={costClass}>
                  <Icons.CurrencyDollar width={12} height={12} />
                </div>
              )}
              {estimationClass !== null && (
                <div className={estimationClass}>
                  <Icons.ThumbsDown width={12} height={12} />
                </div>
              )}
              {rowsRemovedClass !== null && (
                <div className={rowsRemovedClass}>
                  <Icons.FilterFunnel width={12} height={12} />
                </div>
              )}
              <span className='text-xs text-blue-400'>#{node.nodeId}</span>
            </div>
          </div>
          <div className='font-mono whitespace-nowrap'>
            {node[C.RELATION_NAME] !== undefined && (
              <div className='flex flex-row justify-start items-center space-x-1'>
                <span className='text-gray-400'>on</span>
                {node[C.SCHEMA] !== undefined && (
                  <span className='text-gray-600'>{node[C.SCHEMA]}</span>
                )}
                <span className='text-gray-600'>{node[C.RELATION_NAME]}</span>
                {node[C.ALIAS] !== undefined && (
                  <>
                    <span className='text-gray-400'>as</span>
                    <span className='text-gray-600'>{node[C.ALIAS]}</span>
                  </>
                )}
              </div>
            )}
            {node[C.RELATION_NAME] === undefined && node[C.ALIAS] !== undefined && (
              <div className='flex flex-row justify-start items-center space-x-1 text-ellipsis'>
                <span className='text-gray-400'>on</span>
                <span className='text-gray-600'>{utils.keysToString(node[C.ALIAS])}</span>
              </div>
            )}
            {node[C.GROUP_KEY] !== undefined && (
              <div className='flex flex-row justify-start items-center space-x-1'>
                <span className='text-gray-400'>by</span>
                <span className='text-gray-600'>{utils.keysToString(node[C.GROUP_KEY])}</span>
              </div>
            )}
            {node[C.SORT_KEY] !== undefined && (
              <div className='flex flex-row justify-start items-center space-x-1'>
                <span className='text-gray-400'>by</span>
                <span className='text-gray-600 overflow-hidden text-ellipsis'>{utils.sortKeys(node[C.SORT_KEY], node[C.PRESORTED_KEY])}</span>
              </div>
            )}
            {node[C.JOIN_TYPE] !== undefined && (
              <div className='flex flex-row justify-start items-center space-x-1'>
                <span className='text-gray-600'>{node[C.JOIN_TYPE]}</span>
                <span className='text-gray-400'>join</span>
              </div>
            )}
            {node[C.INDEX_NAME] !== undefined && (
              <div className='flex flex-row justify-start items-center space-x-1'>
                <span className='text-gray-400'>using</span>
                <span className='text-gray-600 overflow-hidden text-ellipsis'>{utils.keysToString(node[C.INDEX_NAME])}</span>
              </div>
            )}
            {node[C.HASH_CONDITION] !== undefined && (
              <div className='flex flex-row justify-start items-center space-x-1'>
                <span className='text-gray-400'>on</span>
                <span className='text-gray-600 overflow-hidden text-ellipsis'>{utils.keysToString(node[C.HASH_CONDITION])}</span>
              </div>
            )}
          </div>
        </div>
      </div>
    </div>
  );

  return (
    <div className='flex justify-center text-xs' ref={ref}>
      <div className='text-center w-[240px]'>
        {false && node[C.SUBPLAN_NAME] && (
          <div className='fixed left-0 right-0 bottom-0 flex justify-center items-center'>
            <div className='font-bold italic px-1 whitespace-nowrap text-xs bg-blue-200 rounded'>
              {node[C.SUBPLAN_NAME]}
            </div>
          </div>
        )}
        {false && workersPlannedCount > 0 && (
          <div className='absolute w-full h-full'>
            {workersPlannedCountReversed.map((idx, i) => (
              <div
                className='absolute w-full h-full border bg-white border rounded'
                style={{
                  top: `${3 * idx}px`,
                  left: `${3 * idx}px`,
                }}
                key={i}
              >{i}</div>
            ))}
          </div>
        )}
        {/*workers div*/}
        {false && <div className='bg-white border border-gray-300 rounded shadow p-1'>
          <div className='flex flex-row justify-between items-center whitespace-nowrap'>
            test
            {false && <div className='flex flex-row justify-start items-center space-x-1 overflow-hidden'>
              <Icons.ChevronUp width={20} height={20} className='flex-shrink-0 flex-grow-0 text-gray-400' />
              <div className='text-xs font-semibold overflow-hidden text-ellipsis'>{nodeName}</div>
            </div>}
          </div>
        </div>}
        {false && <div className='bg-white border border-gray-300 rounded shadow p-1'>
          <div className='flex flex-row justify-between items-center whitespace-nowrap'>
            {false && <div className='flex flex-row justify-start items-center space-x-1 overflow-hidden'>
              <Icons.ChevronUp width={20} height={20} className='flex-shrink-0 flex-grow-0 text-gray-400' />
              <div className='text-xs font-semibold overflow-hidden text-ellipsis'>{nodeName}</div>
            </div>}
            {false && <div className='flex flex-row justify-end items-center space-x-1'>
              {durationClass !== null && (
                <div className={durationClass}>
                  <Icons.Clock width={12} height={12} />
                </div>
              )}
              {costClass !== null && (
                <div className={costClass}>
                  <Icons.CurrencyDollar width={12} height={12} />
                </div>
              )}
              {estimationClass !== null && (
                <div className={estimationClass}>
                  <Icons.ThumbsDown width={12} height={12} className='scale-x-[-1]' />
                </div>
              )}
              {rowsRemovedClass !== null && (
                <div className={rowsRemovedClass}>
                  <Icons.FilterFunnel width={12} height={12} />
                </div>
              )}
              <span className='text-xs text-blue-400'>#{node.nodeId}</span>
            </div>}
          </div>
          {false && <div className='font-mono whitespace-nowrap'>
            {node[C.RELATION_NAME] !== undefined && (
              <div className='flex flex-row justify-start items-center space-x-1'>
                <span className='text-gray-400'>on</span>
                {node[C.SCHEMA] !== undefined && (
                  <span className='text-gray-600'>{node[C.SCHEMA]}</span>
                )}
                <span className='text-gray-600'>{node[C.RELATION_NAME]}</span>
                {node[C.ALIAS] !== undefined && (
                  <>
                    <span className='text-gray-400'>as</span>
                    <span className='text-gray-600'>{node[C.ALIAS]}</span>
                  </>
                )}
              </div>
            )}
            {node[C.RELATION_NAME] === undefined && node[C.ALIAS] !== undefined && (
              <div className='flex flex-row justify-start items-center space-x-1 text-ellipsis'>
                <span className='text-gray-400'>on</span>
                <span className='text-gray-600'>{utils.keysToString(node[C.ALIAS])}</span>
              </div>
            )}
            {node[C.GROUP_KEY] !== undefined && (
              <div className='flex flex-row justify-start items-center space-x-1'>
                <span className='text-gray-400'>by</span>
                <span className='text-gray-600'>{utils.keysToString(node[C.GROUP_KEY])}</span>
              </div>
            )}
            {node[C.SORT_KEY] !== undefined && (
              <div className='flex flex-row justify-start items-center space-x-1'>
                <span className='text-gray-400'>by</span>
                <span className='text-gray-600 overflow-hidden text-ellipsis'>{utils.sortKeys(node[C.SORT_KEY], node[C.PRESORTED_KEY])}</span>
              </div>
            )}
            {node[C.JOIN_TYPE] !== undefined && (
              <div className='flex flex-row justify-start items-center space-x-1'>
                <span className='text-gray-600'>{node[C.JOIN_TYPE]}</span>
                <span className='text-gray-400'>join</span>
              </div>
            )}
            {node[C.INDEX_NAME] !== undefined && (
              <div className='flex flex-row justify-start items-center space-x-1'>
                <span className='text-gray-400'>using</span>
                <span className='text-gray-600 overflow-hidden text-ellipsis'>{utils.keysToString(node[C.INDEX_NAME])}</span>
              </div>
            )}
            {node[C.HASH_CONDITION] !== undefined && (
              <div className='flex flex-row justify-start items-center space-x-1'>
                <span className='text-gray-400'>on</span>
                <span className='text-gray-600 overflow-hidden text-ellipsis'>{utils.keysToString(node[C.HASH_CONDITION])}</span>
              </div>
            )}
          </div>}
        </div>}
      </div>
    </div>
  );

  return (
    <div className='fixed flex justify-center' ref={ref}>
      <div className='w-[240px]'>
        {node[C.SUBPLAN_NAME] && (
          <div className='fixed left-0 right-0 bottom-0 flex justify-center items-center'>
            <div className='font-bold italic px-1 whitespace-nowrap text-xs bg-blue-200 rounded'>
              {node[C.SUBPLAN_NAME]}
            </div>
          </div>
        )}
        {workersPlannedCount > 0 && (
          <div className='absolute w-full h-full'>
            {workersPlannedCountReversed.map((idx, i) => (
              <div
                className='absolute w-full h-full border bg-white border rounded'
                style={{
                  top: `${3 * idx}px`,
                  left: `${3 * idx}px`,
                }}
                key={i}
              >{i}</div>
            ))}
          </div>
        )}
        {/*workers div*/}
        <div className='relative bg-white border border-gray-300 rounded shadow p-1'>
          <div className='flex flex-row justify-between items-center whitespace-nowrap'>
            <div className='flex flex-row justify-start items-center space-x-1 overflow-hidden'>
              <Icons.ChevronUp width={20} height={20} className='flex-shrink-0 flex-grow-0 text-gray-400' />
              <div className='text-xs font-semibold overflow-hidden text-ellipsis'>{nodeName}</div>
            </div>
            <div className='flex flex-row justify-end items-center space-x-1'>
              {durationClass !== null && (
                <div className={durationClass}>
                  <Icons.Clock width={12} height={12} />
                </div>
              )}
              {costClass !== null && (
                <div className={costClass}>
                  <Icons.CurrencyDollar width={12} height={12} />
                </div>
              )}
              {estimationClass !== null && (
                <div className={estimationClass}>
                  <Icons.ThumbsDown width={12} height={12} className='scale-x-[-1]' />
                </div>
              )}
              {rowsRemovedClass !== null && (
                <div className={rowsRemovedClass}>
                  <Icons.FilterFunnel width={12} height={12} />
                </div>
              )}
              <span className='text-xs text-blue-400'>#{node.nodeId}</span>
            </div>
          </div>
          <div className='font-mono whitespace-nowrap'>
            {node[C.RELATION_NAME] !== undefined && (
              <div className='flex flex-row justify-start items-center space-x-1'>
                <span className='text-gray-400'>on</span>
                {node[C.SCHEMA] !== undefined && (
                  <span className='text-gray-600'>{node[C.SCHEMA]}</span>
                )}
                <span className='text-gray-600'>{node[C.RELATION_NAME]}</span>
                {node[C.ALIAS] !== undefined && (
                  <>
                    <span className='text-gray-400'>as</span>
                    <span className='text-gray-600'>{node[C.ALIAS]}</span>
                  </>
                )}
              </div>
            )}
            {node[C.RELATION_NAME] === undefined && node[C.ALIAS] !== undefined && (
              <div className='flex flex-row justify-start items-center space-x-1 text-ellipsis'>
                <span className='text-gray-400'>on</span>
                <span className='text-gray-600'>{utils.keysToString(node[C.ALIAS])}</span>
              </div>
            )}
            {node[C.GROUP_KEY] !== undefined && (
              <div className='flex flex-row justify-start items-center space-x-1'>
                <span className='text-gray-400'>by</span>
                <span className='text-gray-600'>{utils.keysToString(node[C.GROUP_KEY])}</span>
              </div>
            )}
            {node[C.SORT_KEY] !== undefined && (
              <div className='flex flex-row justify-start items-center space-x-1'>
                <span className='text-gray-400'>by</span>
                <span className='text-gray-600 overflow-hidden text-ellipsis'>{utils.sortKeys(node[C.SORT_KEY], node[C.PRESORTED_KEY])}</span>
              </div>
            )}
            {node[C.JOIN_TYPE] !== undefined && (
              <div className='flex flex-row justify-start items-center space-x-1'>
                <span className='text-gray-600'>{node[C.JOIN_TYPE]}</span>
                <span className='text-gray-400'>join</span>
              </div>
            )}
            {node[C.INDEX_NAME] !== undefined && (
              <div className='flex flex-row justify-start items-center space-x-1'>
                <span className='text-gray-400'>using</span>
                <span className='text-gray-600 overflow-hidden text-ellipsis'>{utils.keysToString(node[C.INDEX_NAME])}</span>
              </div>
            )}
            {node[C.HASH_CONDITION] !== undefined && (
              <div className='flex flex-row justify-start items-center space-x-1'>
                <span className='text-gray-400'>on</span>
                <span className='text-gray-600 overflow-hidden text-ellipsis'>{utils.keysToString(node[C.HASH_CONDITION])}</span>
              </div>
            )}
          </div>
        </div>
      </div>
    </div>
  )
};

export default GraphNode;