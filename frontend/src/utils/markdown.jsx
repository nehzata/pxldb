import React from 'react';
import Markdown from 'react-markdown';

const A = ({...props}) => (
  // eslint-disable-next-line jsx-a11y/anchor-has-content
  <a className='underline text-gray-500' {...props} />
);
const Blockquote = ({...props}) => <blockquote className='ml-6' {...props} />;
const Code = ({...props}) => (
  <code
    className='font-mono text-gray-700 bg-gray-100 rounded text-sm p-1'
    {...props}
  />
);
const H1 = ({...props}) => (
  // eslint-disable-next-line jsx-a11y/heading-has-content
  <h1
    className='dark:text-white text-black font-bold text-4xl mb-2 last:mb-0 mt-5'
    {...props}
  />
);
const H2 = ({...props}) => (
  // eslint-disable-next-line jsx-a11y/heading-has-content
  <h2
    className='dark:text-white text-black font-bold text-3xl mb-2 last:mb-0 mt-5'
    {...props}
  />
);
const H3 = ({...props}) => (
  // eslint-disable-next-line jsx-a11y/heading-has-content
  <h3
    className='dark:text-white text-black font-bold text-2xl mb-2 last:mb-0 mt-4'
    {...props}
  />
);
const H4 = ({...props}) => (
  // eslint-disable-next-line jsx-a11y/heading-has-content
  <h4
    className='dark:text-white text-black font-bold text-1xl mb-2 last:mb-0 mt-4'
    {...props}
  />
);
const H5 = ({...props}) => (
  // eslint-disable-next-line jsx-a11y/heading-has-content
  <h5 className='dark:text-white text-black font-bold text-lg mb-2 last:mb-0 mt-2' {...props} />
);
const H6 = ({...props}) => (
  // eslint-disable-next-line jsx-a11y/heading-has-content
  <h6 className='dark:text-white text-black font-bold mb-2 last:mb-0 mt-2' {...props} />
);
const P = ({...props}) => <p className='mb-2 last:mb-0' {...props} />;
const Pre = ({...props}) => (
  <pre
    className='block font-mono text-sm mb-2 last:mb-0 p-1 text-gray-700 bg-gray-100 rounded overflow-auto'
    {...props}
  />
);
const Ol = ({...props}) => (
  <ol
    className='list-decimal list-outside ml-10 mb-2 last:mb-0 -mt-2'
    {...props}
  />
);
const Ul = ({...props}) => (
  <ul
    className='list-disc list-outside ml-10 mb-2 last:mb-0 -mt-2'
    {...props}
  />
);

export default ({children}) => (
  <Markdown
    children={children}
    components={{
      a: ({node, ...props}) => <A {...props} />,
      blockquote: ({node, ...props}) => <Blockquote {...props} />,
      code: ({node, ...props}) => <Code {...props} inline='true' />,
      h1: ({node, ...props}) => <H1 {...props} />,
      h2: ({node, ...props}) => <H2 {...props} />,
      h3: ({node, ...props}) => <H3 {...props} />,
      h4: ({node, ...props}) => <H4 {...props} />,
      h5: ({node, ...props}) => <H5 {...props} />,
      h6: ({node, ...props}) => <H6 {...props} />,
      p: ({node, ...props}) => <P {...props} />,
      ol: ({node, ...props}) => <Ol {...props} ordered='true' />,
      ul: ({node, ...props}) => <Ul {...props} ordered={undefined} />,
      pre: ({node, ...props}) => <Pre {...props} />,
    }}
  />
);
