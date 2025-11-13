#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Análisis de conservación de energía y momento
Lee del archivo simulacion.txt
Práctica 5 - Informática II - Universidad de Antioquia
"""

import matplotlib.pyplot as plt
import numpy as np

print("========================================")
print("  ANÁLISIS FÍSICO")
print("========================================\n")

print("Cargando datos de simulacion.txt...")
try:
    with open("simulacion.txt", 'r') as f:
        lineas = f.readlines()
except FileNotFoundError:
    print("ERROR: No se encontró el archivo 'simulacion.txt'")
    exit()

# Extraer trayectorias
trayectorias = []
for linea in lineas:
    if linea.startswith("trayectoria,"):
        partes = linea.strip().split(",")
        trayectorias.append({
            'tiempo': float(partes[1]),
            'id': int(partes[2]),
            'vx': float(partes[5]),
            'vy': float(partes[6]),
            'masa': float(partes[7]),
            'activa': int(partes[9])
        })

if len(trayectorias) == 0:
    print("No hay datos de trayectorias")
    exit()

print(f"✓ {len(trayectorias)} puntos analizados\n")

# Agrupar por tiempo
tiempos_dict = {}
for punto in trayectorias:
    t = punto['tiempo']
    if t not in tiempos_dict:
        tiempos_dict[t] = []
    if punto['activa'] == 1:
        tiempos_dict[t].append(punto)

# Calcular energía y momento
tiempos = sorted(tiempos_dict.keys())
energia_total = []
momento_x_total = []
momento_y_total = []

for t in tiempos:
    puntos = tiempos_dict[t]

    # Energía cinética: E = 0.5 * m * v^2
    E = sum(0.5 * p['masa'] * (p['vx']**2 + p['vy']**2) for p in puntos)
    energia_total.append(E)

    # Momento lineal: p = m * v
    px = sum(p['masa'] * p['vx'] for p in puntos)
    py = sum(p['masa'] * p['vy'] for p in puntos)
    momento_x_total.append(px)
    momento_y_total.append(py)

# Crear gráficos
fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10, 8))

# Gráfico de energía
ax1.plot(tiempos, energia_total, 'b-', linewidth=2)
ax1.set_ylabel('Energía Cinética Total', fontsize=12)
ax1.set_title('Evolución de la Energía (decrece por colisiones inelásticas)',
              fontsize=13, fontweight='bold')
ax1.grid(True, alpha=0.3)

# Gráfico de momento
ax2.plot(tiempos, momento_x_total, 'r-', linewidth=2, label='Momento X')
ax2.plot(tiempos, momento_y_total, 'g-', linewidth=2, label='Momento Y')
ax2.set_xlabel('Tiempo (s)', fontsize=12)
ax2.set_ylabel('Momento Lineal Total', fontsize=12)
ax2.set_title('Conservación del Momento (debe ser aproximadamente constante)',
              fontsize=13, fontweight='bold')
ax2.legend(fontsize=10)
ax2.grid(True, alpha=0.3)

plt.tight_layout()
plt.savefig('analisis_fisico.png', dpi=300, bbox_inches='tight')

# Estadísticas
print("Estadísticas:")
print(f"  Energía inicial: {energia_total[0]:.2f}")
print(f"  Energía final: {energia_total[-1]:.2f}")
print(f"  Pérdida: {energia_total[0] - energia_total[-1]:.2f} ({100*(1-energia_total[-1]/energia_total[0]):.1f}%)")
print(f"\n  Momento X promedio: {np.mean(momento_x_total):.2f} (σ={np.std(momento_x_total):.2f})")
print(f"  Momento Y promedio: {np.mean(momento_y_total):.2f} (σ={np.std(momento_y_total):.2f})")

print(f"\n✓ Gráfica guardada: analisis_fisico.png")
print("========================================")
plt.show()
