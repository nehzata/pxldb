import React from 'react';
import * as Icons from 'untitledui-js-base';

import * as C from './consts';
import * as utils from './utils';


const Chart = ({plan, metric}) => {
  const [plans, maxEstimateFactor] = React.useMemo(() => {
    const res = [[]];
    utils.flatten(res[0], 0, plan.content.Plan, true, []);
    plan.ctes.forEach(cte => {
      const flat = []
      utils.flatten(flat, 0, cte, true, [])
      res.push(flat)
    });
    const maxEstimateFactor = res.reduce((acc, p) => p.reduce((acc, row) => {
      const f = row[1][C.PLANNER_ESTIMATE_FACTOR];
      if (f !== Infinity) {
        return Math.max(acc, f);
      }
      return acc;
    }, acc), 0);
    return [res, maxEstimateFactor];
  }, [plan]);

  return (
    <div className='flex flex-col space-y-4'>
      {plans.map((flat, i) => (
        <div key={i}>
          {flat.map((row, j) => (
            <React.Fragment key={j}>
              {row[1][C.SUBPLAN_NAME] !== undefined && (
                <div className='ml-8 italic'>
                  {row[1][C.SUBPLAN_NAME]}
                </div>
              )}
              <div className='grid grid-cols-12 gap-2 items-center'>
                <div className='col-span-5 flex flex-row justify-start items-center'>
                  <div className='flex-shrink-0 w-8 text-sm'>#{row[1].nodeId}</div>
                  <div
                    className='whitespace-nowrap overflow-hidden text-ellipsis'
                    style={{marginLeft: `${row[0]}rem`}}
                  >
                    {row[1][C.NODE_TYPE]}
                  </div>
                </div>
                <div className='col-span-7'>
                  <div className='h-1'>
                    {metric === 'time' && (
                      <div
                        className={row[1][C.EXCLUSIVE_DURATION] > 0
                          ? 'h-1 border-l border-gray-500 bg-gray-500'
                          : 'h-1'
                        }
                        style={{
                          width: `${(row[1][C.EXCLUSIVE_DURATION] / (plan.planStats.executionTime || plan.content.Plan[C.ACTUAL_TOTAL_TIME])) * 100}%`,
                        }}
                      />
                    )}
                    {metric === 'rows' && (
                      <div
                        className='h-1 bg-gray-500'
                        style={{
                          width: `${Math.round(row[1][C.ACTUAL_ROWS_REVISED] / plan.planStats.maxRows) * 100}%`,
                        }}
                      />
                    )}
                    {metric === 'estimation' && (
                      <div className='flex flex-row justify-center items-center'>
                        <div className='w-2 mr-2 flex-shrink-0'>
                          {row[1][C.PLANNER_ESTIMATE_DIRECTION] === C.PLANNER_ESTIMATE_DIRECTION_UNDER && <Icons.ArrowDown size={12} stroke='currentColor' />}
                        </div>
                        <div
                          className={row[1][C.PLANNER_ESTIMATE_DIRECTION] === C.PLANNER_ESTIMATE_DIRECTION_UNDER ? 'h-1 bg-gray-500' : 'h-1 bg-gray-300'}
                          style={{
                            width: `${Math.min(100, (row[1][C.PLANNER_ESTIMATE_FACTOR] || 0) / maxEstimateFactor * 100)}%`,
                          }}
                        />
                        <div className='h-2 w-[1px] bg-gray-500' />
                        <div
                          className={row[1][C.PLANNER_ESTIMATE_DIRECTION] === C.PLANNER_ESTIMATE_DIRECTION_OVER ? 'h-1 bg-gray-500' : 'h-1 bg-gray-300'}
                          style={{
                            width: `${Math.min(100, (row[1][C.PLANNER_ESTIMATE_FACTOR] || 0) / maxEstimateFactor * 100)}%`,
                          }}
                        />
                        <div className='w-2 ml-2 flex-shrink-0'>
                          {row[1][C.PLANNER_ESTIMATE_DIRECTION] === C.PLANNER_ESTIMATE_DIRECTION_OVER && <Icons.ArrowUp size={12} stroke='currentColor' />}
                        </div>
                      </div>
                    )}
                    {metric === 'cost' && (
                      <div
                        className={row[1][C.EXCLUSIVE_COST] > 0
                          ? 'h-1 border-l border-gray-500 bg-gray-500'
                          : 'h-1'
                        }
                        style={{
                          width: `${Math.round(row[1][C.EXCLUSIVE_COST] / plan.planStats.maxCost * 100)}%`,
                        }}
                      />
                    )}
                  </div>
                </div>
              </div>
            </React.Fragment>
          ))}
        </div>
      ))}
    </div>
  );
};

export default Chart;

