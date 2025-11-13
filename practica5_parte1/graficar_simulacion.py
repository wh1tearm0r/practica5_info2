#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Script para graficar las trayectorias de la simulación
Lee del archivo simulacion.txt generado por el programa C++
Práctica 5 - Informática II - Universidad de Antioquia
"""

import matplotlib.pyplot as plt
import numpy as np

print("========================================")
print("  GRAFICADOR DE TRAYECTORIAS")
print("  Práctica 5 - Informática II")
print("========================================\n")

# Leer archivo único
print("Cargando datos de simulacion.txt...")
try:
    with open("simulacion.txt", 'r') as f:
        lineas = f.readlines()
except FileNotFoundError:
    print("ERROR: No se encontró el archivo 'simulacion.txt'")
    print("Ejecuta primero la simulación en C++")
    exit()

# Extraer datos de diferentes secciones
trayectorias = []
obstaculos = []
box_ancho = 800
box_alto = 600

for linea in lineas:
    linea = linea.strip()

    # Leer dimensiones del box
    if linea.startswith("# Box:"):
        partes = linea.split(":")
        dims = partes[1].strip().split("x")
        box_ancho = float(dims[0])
        box_alto = float(dims[1])

    # Leer trayectorias
    if linea.startswith("trayectoria,"):
        partes = linea.split(",")
        trayectorias.append({
            'tiempo': float(partes[1]),
            'id': int(partes[2]),
            'x': float(partes[3]),
            'y': float(partes[4]),
            'vx': float(partes[5]),
            'vy': float(partes[6]),
            'masa': float(partes[7]),
            'radio': float(partes[8]),
            'activa': int(partes[9])
        })

    # Leer obstáculos
    if linea.startswith("obstaculo,"):
        partes = linea.split(",")
        obstaculos.append({
            'id': int(partes[1]),
            'x': float(partes[2]),
            'y': float(partes[3]),
            'ancho': float(partes[4]),
            'alto': float(partes[5])
        })

print(f"✓ Datos cargados:")
print(f"  - Puntos de trayectoria: {len(trayectorias)}")
print(f"  - Obstáculos: {len(obstaculos)}")
print(f"  - Dimensiones box: {box_ancho} x {box_alto}\n")

# Crear figura
plt.figure(figsize=(12, 9))

# Dibujar obstáculos
print("Dibujando obstáculos...")
for obs in obstaculos:
    plt.gca().add_patch(plt.Rectangle((obs['x'], obs['y']),
                                      obs['ancho'], obs['alto'],
                                      facecolor='gray',
                                      edgecolor='black',
                                      linewidth=2,
                                      alpha=0.5))

# Dibujar límite de la caja
plt.gca().add_patch(plt.Rectangle((0, 0), box_ancho, box_alto,
                                  facecolor='none',
                                  edgecolor='black',
                                  linewidth=3))

# Colores para cada partícula
colores = ['red', 'blue', 'green', 'orange', 'purple', 'brown', 'pink', 'cyan']

# Agrupar trayectorias por partícula
print("Graficando trayectorias...")
particulas_dict = {}
for punto in trayectorias:
    pid = punto['id']
    if pid not in particulas_dict:
        particulas_dict[pid] = []
    if punto['activa'] == 1:  # Solo puntos donde la partícula está activa
        particulas_dict[pid].append(punto)

# Graficar cada partícula
for pid, puntos in particulas_dict.items():
    if len(puntos) == 0:
        continue

    color = colores[pid % len(colores)]

    # Extraer coordenadas
    x = [p['x'] for p in puntos]
    y = [p['y'] for p in puntos]

    # Dibujar trayectoria
    plt.plot(x, y, color=color, linewidth=2, alpha=0.7, label=f'Partícula {pid}')

    # Marcar inicio (círculo)
    plt.plot(x[0], y[0], 'o', color=color, markersize=12,
            markeredgecolor='black', markeredgewidth=2)

    # Marcar final (cuadrado)
    plt.plot(x[-1], y[-1], 's', color=color, markersize=12,
            markeredgecolor='black', markeredgewidth=2)

    print(f"  ✓ Partícula {pid}: {len(puntos)} puntos graficados")

# Configurar gráfico
plt.title("Trayectoria de las partículas (Movimiento con Colisiones Múltiples)",
         fontsize=14, fontweight='bold')
plt.xlabel("Posición X", fontsize=12)
plt.ylabel("Posición Y", fontsize=12)
plt.xlim(0, box_ancho)
plt.ylim(0, box_alto)
plt.gca().set_aspect('equal')
plt.grid(True, alpha=0.3)
plt.legend(loc='upper right', fontsize=10)

# Guardar y mostrar
plt.tight_layout()
plt.savefig('trayectorias_simulacion.png', dpi=300, bbox_inches='tight')
print(f"\n✓ Gráfica guardada: trayectorias_simulacion.png")

print("\n========================================")
plt.show()
