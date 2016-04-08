#!/usr/bin/python
#coding=utf-8
import os,psycopg2,time,uuid,traceback
import MySQLdb
from computer_info import getCPUtemperature,getRAMinfo,getCPUuse,getDiskSpace,get_gpu_temp

def get_uuid():
    """
    # 获取新的uuid
    """
    return uuid.uuid4().hex
try:
    # CPU 温度
    CPU_temp = getCPUtemperature()
    # gpu 温度
    GPU_temp = get_gpu_temp()
    # CPU 使用率
    CPU_usage = getCPUuse()
    print CPU_usage
    # 获取室内湿度，温度
    house_info = os.popen('/home/ftp/house').readlines()
    print house_info
    house_rh = 0
    house_temp = 0
    if house_info != '(null)':
        # 室内湿度
        house_rh = house_info[0].split(',')[0]
        # 温度
        house_temp = house_info[0].split(',')[1]
    
    # RAM information
    # Output is in kb, here I convert it in Mb for readability
    RAM_stats = getRAMinfo()
    
    #RAM总和
    RAM_total = round(int(RAM_stats[0]) / 1000,1)
    #RAM使用
    RAM_used = round(int(RAM_stats[1]) / 1000,1)
    #RAM空闲
    RAM_free = round(int(RAM_stats[2]) / 1000,1)
    
    # Disk information
    DISK_stats = getDiskSpace()
    # 磁盘总和
    DISK_total = DISK_stats[0]
    # 磁盘使用
    DISK_used = DISK_stats[1]
    # 磁盘使用率
    DISK_perc = DISK_stats[3]
    #RAM信息[总量，使用,使用率]
    ram_info = [RAM_total,RAM_used,round(int(RAM_used) / RAM_total,2)]
    #硬盘信息[总量，使用,使用率]
    disk_info = [float(DISK_total[:-1]),float(DISK_used[:-1]),float(DISK_perc[:-1])]
    now = time.strftime("%Y-%m-%d %H:%M:%S",time.localtime(time.time()))
#   db = psycopg2.connect(database='postgres', user='postgres', password='123456', host='127.0.0.1', port='5432')

    db = MySQLdb.connect("localhost","root","margin","marginer" )
    cursor = db.cursor()
    sql = """insert into marginer_info (id, temperature, humidity, cpu_temperature, gpu_temperature, cpu_rate, ram, disk, time)
             VALUES ('%s','%s','%s','%s','%s','%s','%s','%s','%s')""" % (get_uuid(),house_temp,house_rh,CPU_temp,GPU_temp,CPU_usage,ram_info,disk_info,now)
    log = open("log.txt","a+")
    log.write(sql)
    log.close()
    print sql
    cursor.execute(sql)
    db.commit()
    cursor.close()
    db.close()
except:
    print(traceback.format_exc())
