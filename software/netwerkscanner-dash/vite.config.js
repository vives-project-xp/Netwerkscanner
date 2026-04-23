import { defineConfig, loadEnv } from 'vite'
import react from '@vitejs/plugin-react'
import tailwindcss from '@tailwindcss/vite'

export default defineConfig(({ mode }) => {
  const env = loadEnv(mode, '.', '')

  return {
    plugins: [
      react(),
      tailwindcss(),
    ],
    server: {
      host: true,
      proxy: {
        '/api': {
          target: 'http://10.20.10.24:8081',
          changeOrigin: true,
          rewrite: (path) => path.replace(/^\/api/, '')
        },
        '/upload': {
          target: env.VITE_UPLOAD_SERVER || 'http://localhost:3030',
          changeOrigin: true
        },
        '/assets/projectdata': {
          target: env.VITE_UPLOAD_SERVER || 'http://localhost:3030',
          changeOrigin: true
        },
        '/projects': {
          target: env.VITE_UPLOAD_SERVER || 'http://localhost:3030',
          changeOrigin: true
        }
      }
    }
  }
})