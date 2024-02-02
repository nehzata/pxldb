import React from 'react';
import Logo from '@pxl/assets/logo.svg';


const Empty = () => (
  <div className='flex flex-col justify-start items-center h-full'>
    <div
      className='w-full h-[40px]'
      style={{'--wails-draggable': 'drag'}}
    />
    <div className='flex-grow flex flex-col justify-center items-center'>
      <Logo className='w-1/4 h-1/4 text-gray-100 mb-8 -mt-[40px]' />
      <div className='grid grid-cols-2 gap-x-4 gap-y-2 text-sm'>
        <div className='text-gray-400 text-right'>Find a session</div>
        <div className='flex flex-row justify-start items-center space-x-2'>
          <div className='text-gray-500 bg-gray-100 text-center border rounded px-2'>&#8984;</div>
          <div className='text-gray-500 bg-gray-100 text-center border rounded px-2'>T</div>
        </div>
        <div className='text-gray-400 text-right'>Execute a query</div>
        <div className='flex flex-row justify-start items-center space-x-2'>
          <div className='text-gray-500 bg-gray-100 text-center border rounded px-2'>&#8984;</div>
          <div className='text-gray-500 bg-gray-100 text-center border rounded px-2'>enter</div>
        </div>
        <div className='text-gray-400 text-right'>Quick copy & paste</div>
        <div className='flex flex-row justify-start items-center space-x-2'>
          <div className='text-gray-500 bg-gray-100 text-center border rounded px-2'>&#8984;</div>
          <div className='text-gray-500 bg-gray-100 text-center border rounded px-2'>click</div>
        </div>
        <div className='text-gray-400 text-right'>Quick copy</div>
        <div className='flex flex-row justify-start items-center space-x-2'>
          <div className='text-gray-500 bg-gray-100 text-center border rounded px-2'>alt</div>
          <div className='text-gray-500 bg-gray-100 text-center border rounded px-2'>click</div>
        </div>
      </div>
    </div>
  </div>
);

export default Empty;