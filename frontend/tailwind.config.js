/** @type {import('tailwindcss').Config} */
export default {
  content: ['./src/**/*.{js,jsx,html}'],
  theme: {
    extend: {
      animation: {
        'spin-ccw': 'spin-ccw 1s linear infinite',
      },
      keyframes: {
        'spin-ccw': {
          from: {
            transform: 'rotate(360deg)',
          },
          to: {
            transform: 'rotate(0deg)',
          }
        }
      },
      colors: {
        canvas: {
          dark: '#1d1d1d',
          DEFAULT: '#ffffff',
        },
        subcanvas: {
          dark: '#171717',
          DEFAULT: '#f7f7f7',
        },
        control: {
          dark: '#1e1e1e',
          DEFAULT: '#ffffff',
        },
        divider: {
          // dark: 'rgba(255,255,255,0.12)',
          dark: '#2a2a2a',
          DEFAULT: 'rgba(0,0,0,0.12)',
        },
      },
      transitionProperty: {
        'spacing': 'margin, padding',
      }
    },
  },
  plugins: [],
};
