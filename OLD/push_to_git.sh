#!/bin/bash
cd "$(dirname "$0")"
git init
git remote add origin https://github.com/edziller/NINA-ASCOM-Safety-Monitor.git
git add .
git commit -m "Primeiro commit do projeto: estação meteorológica para NINA Safety Monitor"
git branch -M main
git push -u origin main
