# coding=utf-8

"""

Copyright(c) 2022-2023 Max Qian  <lightapt.com>

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License version 3 as published by the Free Software Foundation.
This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.
You should have received a copy of the GNU Library General Public License
along with this library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
Boston, MA 02110-1301, USA.

"""

import os
import logging

def create_logger(name : str) -> logging.Logger:
    """
        Create a logger instance for the given name.
        Args : name : str # usually the name of the module
        Returns : logging.Logger
    """
    logger = logging.getLogger(name)
    # 创建一个handler，用于写入日志文件
    filename = f'logs/{name}.log'
    fh = logging.FileHandler(filename, mode='w+', encoding='utf-8')
    ch = logging.StreamHandler()
    formatter = logging.Formatter('[%(asctime)s][%(name)s][%(levelname)s] - %(message)s')
    logger.setLevel(logging.DEBUG)
    fh.setFormatter(formatter)
    ch.setFormatter(formatter)
    logger.addHandler(fh)
    logger.addHandler(ch)
    return logger

# if the log folder is not exist, create it
if not os.path.exists("./logs"):
    os.mkdir("./logs")

lightguider_logger = create_logger("lightguier")