import * as d3 from 'd3';


export const flatten = (output, level, node, isLast, branches) => {
  // [level, node, isLastSibbling, branches]
  output.push([level, node, isLast, [...branches]]);
  if (!isLast) {
    branches.push(level)
  }

  const plans = node.Plans || [];
  const last = plans.length ? plans[plans.length - 1] : null;

  plans.forEach(subnode => {
    flatten(
      output,
      level + 1,
      subnode,
      subnode === last,
      branches
    );
  });
  if (!isLast) {
    branches.pop();
  }
};


export const duration = value => {
  if (value === undefined) {
    return "N/A";
  }
  const result = [];
  let denominator = 1000 * 60 * 60 * 24;
  const days = Math.floor(value / denominator);
  if (days) {
    result.push(days + "d");
  }
  let remainder = value % denominator;
  denominator /= 24;
  const hours = Math.floor(remainder / denominator);
  if (hours) {
    result.push(hours + "h");
  }
  remainder = remainder % denominator;
  denominator /= 60;
  const minutes = Math.floor(remainder / denominator);
  if (minutes) {
    result.push(minutes + "m");
  }
  remainder = remainder % denominator;
  denominator /= 60;
  const seconds = Math.floor(remainder / denominator);
  if (seconds) {
    result.push(seconds + "s");
  }
  remainder = remainder % denominator;
  const milliseconds = parseFloat(remainder.toPrecision(3));
  result.push(milliseconds.toLocaleString() + "ms");

  return result.slice(0, 2).join(" ");
};


export const getLayoutExtent = node => {
  const descs = node.descendants();
  const {minX, maxX, minY, maxY} = descs.reduce((acc, childNode) => {
    // console.log(childNode.x, childNode.xSize, childNode.y, childNode.ySize);
    return {
      minX: Math.min(acc.minX, childNode.x - childNode.xSize / 2) || 0,
      maxX: Math.max(acc.maxX, childNode.x + childNode.xSize / 2) || 0,
      minY: Math.min(acc.minY, childNode.y) || 0,
      maxY: Math.max(acc.maxY, childNode.y + childNode.ySize) || 0,
    };
  }, {minX: Infinity, maxX: 0, minY: Infinity, maxY: 0});
  return [minX, maxX, minY, maxY];
};


export const lineGen = ({source, target}, padding) => {
  const path = d3.path();
  path.moveTo(source.x, source.y + source.ySize - padding / 2);
  path.bezierCurveTo(
    source.x,
    source.y + source.ySize + padding / 2,
    target.x,
    target.y - padding / 2,
    target.x,
    target.y + padding / 2
  );
  return path.toString();
};


export const edgeWidth = (maxRows) => d3
  .scaleLinear()
  .domain([0, maxRows])
  .range([1, 25 / 1.5]);



export const keysToString = (value) => {
  let r = value;
  if (!(r instanceof Array)) {
    r = [r];
  }
  r = r.map(v => v.replace(/(^\(|\)$)/g, ''));
  return r.join(', ');
};


export const sortKeys = (sort, presort) => {
  return sort.map((v) => {
    let result = v;
    if (presort) {
      result +=
        presort.indexOf(v) !== -1
          ? `&nbsp;<span class='text-muted'>(presort)</span>`
          : '';
    }
    return result;
  }).join(', ');
};