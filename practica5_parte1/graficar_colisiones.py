#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Script para graficar el historial de colisiones
Lee del archivo simulacion.txt
Práctica 5 - Informática II - Universidad de Antioquia
"""

import matplotlib.pyplot as plt

print("========================================")
print("  ANÁLISIS DE COLISIONES")
print("========================================\n")

print("Cargando datos de simulacion.txt...")
try:
    with open("simulacion.txt", 'r') as f:
        lineas = f.readlines()
except FileNotFoundError:
    print("ERROR: No se encontró el archivo 'simulacion.txt'")
    exit()

# Extraer colisiones
colisiones = []
for linea in lineas:
    if linea.startswith("colision,"):
        partes = linea.strip().split(",")
        colisiones.append({
            'tiempo': float(partes[1]),
            'tipo': partes[2],
            'id1': int(partes[3]),
            'id2': int(partes[4]),
            'detalles': partes[5] if len(partes) > 5 else ""
        })

if len(colisiones) == 0:
    print("No hay colisiones registradas en la simulación")
    exit()

print(f"✓ {len(colisiones)} colisiones detectadas\n")

# Contar tipos
tipos_contador = {}
for col in colisiones:
    tipo = col['tipo']
    tipos_contador[tipo] = tipos_contador.get(tipo, 0) + 1

print("Distribución de colisiones:")
for tipo, conteo in tipos_contador.items():
    print(f"  - {tipo}: {conteo}")

# Crear gráfico
plt.figure(figsize=(10, 6))

colores_barras = ['red', 'blue', 'green', 'orange', 'purple', 'cyan', 'magenta']
plt.bar(range(len(tipos_contador)), list(tipos_contador.values()),
       color=colores_barras[:len(tipos_contador)],
       edgecolor='black',
       linewidth=1.5)

plt.xticks(range(len(tipos_contador)), list(tipos_contador.keys()), rotation=45, ha='right')
plt.ylabel('Número de colisiones', fontsize=12)
plt.title('Distribución de Colisiones por Tipo', fontsize=14, fontweight='bold')
plt.grid(axis='y', alpha=0.3)
plt.tight_layout()

plt.savefig('histograma_colisiones.png', dpi=300, bbox_inches='tight')
print(f"\n✓ Gráfica guardada: histograma_colisiones.png")
print("========================================")
plt.show()
