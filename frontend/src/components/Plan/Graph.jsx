import React from 'react';
import * as d3 from 'd3';
import {flextree} from 'd3-flextree';

import * as C from './consts';
import * as utils from './utils';
import GraphNode from './GraphNode';


const padding = 40;
const minScale = 0.2;
const layout = flextree({
  nodeSize: node => {
    if (node.data.size) {
      return [node.data.size[0] + padding, node.data.size[1] + padding];
    }
    return [0, 0];
  },
  spacing: (nodeA, nodeB) => Math.pow(nodeA.path(nodeB).length, 1.5),
});


const Graph = ({rootNode, plan}) => {
  const [zoomListener, setZoomListener] = React.useState(null);
  const [transform, setTransform] = React.useState({
    t: '',
    scale: 1,
  });

  const [nodeSizes, setNodeSizes] = React.useState({});

  const ref = React.useRef(null);
  const edgeWeight = React.useMemo(() => utils.edgeWidth(plan.planStats.maxRows));
  const [root, ctes] = React.useMemo(() => {
    const root = layout.hierarchy(rootNode, v => v.Plans);
    const ctes = plan?.ctes.map(cte => layout.hierarchy(cte, v => v.Plans));
    return [root, ctes];
  }, [rootNode, plan]);

  const [rootLayout, rootNodes, cteNodes] = React.useMemo(() => {
    root.data.size = nodeSizes[root.data.nodeId];
    root.each(n => {
      n.data.size = nodeSizes[n.data.nodeId];
    });

    ctes.forEach(cte => {
      cte.data.size = nodeSizes[cte.data.nodeId];
      cte.each(node => {
        node.data.size = nodeSizes[node.data.nodeId];
      });
    });

    const rootLayout = layout(root);
    const rootNodes = rootLayout.descendants();

    const rootLayoutExtent = utils.getLayoutExtent(rootLayout);

    const {list: cteNodes} = ctes.reduce((acc, cte) => {
      const node = layout(cte);

      const _extent = utils.getLayoutExtent(node);
      const width = _extent[1] - _extent[0];
      const height = _extent[3] - _extent[2];
      node.each((node) => {
        node.x += acc.offset[0] - _extent[0];
        node.y += acc.offset[1];
      });

      acc.offset[0] += width;
      acc.offset[1] += height;

      acc.list.push({
        node,
        extent: utils.getLayoutExtent(node),
        descendants: node.descendants(),
        links: node.links().map(link => ({
          ...link,
          line: utils.lineGen(link, padding),
          width: edgeWeight(link.target.data[C.ACTUAL_ROWS_REVISED]),
        }))
      });

      return acc;
    }, {offset: [rootLayoutExtent[0], rootLayoutExtent[3] + padding], list: []});

    return [rootLayout, rootNodes, cteNodes];
  }, [root, nodeSizes]);

  const rootLinks = React.useMemo(() => {
    return rootLayout.links().map(link => ({
      ...link,
      line: utils.lineGen(link, padding),
      width: edgeWeight(link.target.data[C.ACTUAL_ROWS_REVISED]),
    }));
  }, [rootLayout, nodeSizes]);

  const toCteLinks = React.useMemo(() => {
    const _toCteLinks = [];

    // root CTE links
    rootNodes.forEach(source => {
      if (source.data.hasOwnProperty(C.CTE_NAME)) {
        const sname = `CTE ${source.data[C.CTE_NAME]}`;
        const cte = ctes.reduce((acc, node) => {
          if (acc === null && node.data[C.SUBPLAN_NAME] === sname) {
            return node;
          }
          return acc;
        }, null);
        if (cte !== null) {
          _toCteLinks.push({
            source,
            target: cte,
          });
        }
      }
    });

    cteNodes.forEach(({descendants}) => {
      descendants.forEach(source => {
        if (source.data.hasOwnProperty(C.CTE_NAME)) {
          const sname = `CTE ${source.data[C.CTE_NAME]}`;
          const cte = ctes.reduce((acc, node) => {
            if (acc === null && node.data[C.SUBPLAN_NAME] === sname) {
              return node;
            }
            return acc;
          }, null);
          if (cte !== null) {
            _toCteLinks.push({
              source,
              target: cte,
            });
          }
        }
      });
    });

    const toCteLinks = _toCteLinks.map(link => ({
      ...link,
      line: utils.lineGen(link, padding),
      width: edgeWeight(link.target.data[C.ACTUAL_ROWS_REVISED]),
    }));

    return toCteLinks;
  }, [rootNodes]);

  const updateNodeSize = React.useCallback((id, width, height) => {
    setNodeSizes(ns => ({
      ...ns,
      [id]: [width / transform.scale, height / transform.scale],
    }));
  }, [transform.scale, setNodeSizes]);


  React.useEffect(() => {
    setZoomListener(() => {
      const zl = d3
        .zoom()
        .scaleExtent([minScale, 3])
        .on('zoom', e => {
          setTransform({
            t: e.transform,
            scale: e.transform.k,
          });
        });
      d3.select(ref.current).call(zl)
      if (rootLayout) {
        const [x0, x1, y0, y1] = utils.getLayoutExtent(rootLayout);
        const rect = ref.current.getBoundingClientRect();

        d3.select(ref.current)
          .transition()
          .call(
            zl.transform,
            d3.zoomIdentity
              .translate(rect.width / 2, 10)
              .scale(
                Math.min(
                  1,
                  Math.max(
                    minScale,
                    0.8 /
                    Math.max((x1 - x0) / rect.width, (y1 - y0) / rect.height)
                  )
                )
              )
              .translate(-(x0 + x1) / 2, 10)
          );
      }
      return zl;
    });
  }, []);

  return (
    <svg width='100%' height='100%' ref={ref}>
      <g transform={transform.t} >
        {toCteLinks.map((link, i) => (
          <path
            d={link.line}
            stroke='#b3d7d7'
            strokeWidth={link.width}
            fill='none'
            key={i}
          />
        ))}
        {rootLinks.map((link, i) => (
          <path
            d={link.line}
            stroke='grey'
            strokeWidth={link.width}
            strokeLinecap='square'
            fill='none'
            key={i}
          />
        ))}
        {rootNodes.map((item, i) => (
          <foreignObject
            x={item.x - item.xSize / 2}
            y={item.y + padding / 2}
            width={item.xSize}
            height={item.ySize - padding}
            key={i}
          >
            <GraphNode plan={plan} node={item.data} onSize={updateNodeSize.bind(null, item.data.nodeId)} />
          </foreignObject>
        ))}
        {cteNodes.map(({extent, descendants, links}, i) => (
          <g key={i}>
            <rect
              x={extent[0]}
              y={extent[2]}
              width={extent[1] - extent[0]}
              height={extent[3] - extent[2]}
              stroke='#cfcfcf'
              strokeWidth='2'
              fill='#cfcfcf'
              fillOpacity='10%'
              rx='5'
              ry='5'
            />
            {links.map((link, i) => (
              <path
                d={link.line}
                stroke='grey'
                strokeWidth={link.width}
                strokeLinecap='square'
                fill='none'
                key={i}
              />
            ))}
            {descendants.map((item, i) => (
              <foreignObject
                x={item.x - item.xSize / 2}
                y={item.y + padding / 2}
                width={item.xSize}
                height={item.ySize - padding}
                key={i}
              >
                <GraphNode plan={plan} node={item.data} onSize={updateNodeSize.bind(null, item.data.nodeId)} />
              </foreignObject>
            ))}
          </g>
        ))}
      </g>
    </svg>
  );
};

export default Graph;