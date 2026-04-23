# Gebruik een Node.js image
FROM node:20-alpine

# Maak werkdirectory aan
WORKDIR /app

# Kopieer package.json en package-lock.json
COPY package.json package-lock.json* ./

# Installeer dependencies
RUN npm install

# Kopieer de rest van de app
COPY . .

# Expose de Vite dev server en uploadserver poorten
EXPOSE 5173 3030

# Start de dev server
CMD ["npm", "run", "dev"]
