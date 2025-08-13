/*
 * Copyright 2025 KODE DIY, SOCIEDAD LIMITADA
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef KODE_MAX31329_REGISTERS_H
#define KODE_MAX31329_REGISTERS_H

// Device I2C default address (7-bit)
#define MAX31329_I2C_ADDRESS                 0x68

// Register map (see datasheet)
#define MAX31329_REG_STATUS                  0x00
#define MAX31329_REG_INT_EN                  0x01
#define MAX31329_REG_RTC_RESET               0x02
#define MAX31329_REG_CFG1                    0x03
#define MAX31329_REG_CFG2                    0x04
#define MAX31329_REG_TIMER_CONFIG            0x05
#define MAX31329_REG_SECONDS                 0x06
#define MAX31329_REG_MINUTES                 0x07
#define MAX31329_REG_HOURS                   0x08
#define MAX31329_REG_DAY                     0x09
#define MAX31329_REG_DATE                    0x0A
#define MAX31329_REG_MONTH                   0x0B
#define MAX31329_REG_YEAR                    0x0C
#define MAX31329_REG_ALM1_SEC                0x0D
#define MAX31329_REG_ALM1_MIN                0x0E
#define MAX31329_REG_ALM1_HRS                0x0F
#define MAX31329_REG_ALM1DAY_DATE            0x10
#define MAX31329_REG_ALM1_MON                0x11
#define MAX31329_REG_ALM1_YEAR               0x12
#define MAX31329_REG_ALM2_MIN                0x13
#define MAX31329_REG_ALM2_HRS                0x14
#define MAX31329_REG_ALM2DAY_DATE            0x15
#define MAX31329_REG_TIMER_COUNT             0x16
#define MAX31329_REG_TIMER_INIT              0x17
#define MAX31329_REG_PWR_MGMT                0x18
#define MAX31329_REG_TRICKLE                 0x19

#define MAX31329_REG_RAM_START               0x22
#define MAX31329_REG_RAM_END                 0x61

// STATUS bits
#define MAX31329_STATUS_A1F_POS              0
#define MAX31329_STATUS_A2F_POS              1
#define MAX31329_STATUS_TIF_POS              2
#define MAX31329_STATUS_DIF_POS              3
#define MAX31329_STATUS_PFAIL_POS            5
#define MAX31329_STATUS_OSF_POS              6
#define MAX31329_STATUS_PSDECT_POS           7

#define MAX31329_STATUS_A1F                  (1u << MAX31329_STATUS_A1F_POS)
#define MAX31329_STATUS_A2F                  (1u << MAX31329_STATUS_A2F_POS)
#define MAX31329_STATUS_TIF                  (1u << MAX31329_STATUS_TIF_POS)
#define MAX31329_STATUS_DIF                  (1u << MAX31329_STATUS_DIF_POS)
#define MAX31329_STATUS_PFAIL                (1u << MAX31329_STATUS_PFAIL_POS)
#define MAX31329_STATUS_OSF                  (1u << MAX31329_STATUS_OSF_POS)
#define MAX31329_STATUS_PSDECT               (1u << MAX31329_STATUS_PSDECT_POS)

// INT_EN bits
#define MAX31329_INT_A1IE_POS                0
#define MAX31329_INT_A2IE_POS                1
#define MAX31329_INT_TIE_POS                 2
#define MAX31329_INT_DIE_POS                 3
#define MAX31329_INT_PFAILE_POS              5
#define MAX31329_INT_DOSF_POS                6

#define MAX31329_INT_A1IE                    (1u << MAX31329_INT_A1IE_POS)
#define MAX31329_INT_A2IE                    (1u << MAX31329_INT_A2IE_POS)
#define MAX31329_INT_TIE                     (1u << MAX31329_INT_TIE_POS)
#define MAX31329_INT_DIE                     (1u << MAX31329_INT_DIE_POS)
#define MAX31329_INT_PFAILE                  (1u << MAX31329_INT_PFAILE_POS)
#define MAX31329_INT_DOSF                    (1u << MAX31329_INT_DOSF_POS)

// RTC_RESET bits
#define MAX31329_RESET_SWRST                 (1u << 0)

// CFG1 bits
#define MAX31329_CFG1_ENOSC                  (1u << 0)
#define MAX31329_CFG1_I2C_TIMEOUT            (1u << 1)
#define MAX31329_CFG1_DATA_RET               (1u << 2)
#define MAX31329_CFG1_ENIO                   (1u << 3)

// CFG2 bits
#define MAX31329_CFG2_CLKIN_HZ_POS           0
#define MAX31329_CFG2_CLKIN_HZ_MASK          (0x03u << MAX31329_CFG2_CLKIN_HZ_POS)
#define MAX31329_CFG2_ENCLKIN                (1u << 2)
#define MAX31329_CFG2_DIP                    (1u << 3)
#define MAX31329_CFG2_CLKO_HZ_POS            5
#define MAX31329_CFG2_CLKO_HZ_MASK           (0x03u << MAX31329_CFG2_CLKO_HZ_POS)
#define MAX31329_CFG2_ENCLKO                 (1u << 7)

// TIMER_CONFIG bits
#define MAX31329_TMR_TFS_POS                 0
#define MAX31329_TMR_TFS_MASK                (0x03u << MAX31329_TMR_TFS_POS)
#define MAX31329_TMR_TRPT                    (1u << 2)
#define MAX31329_TMR_TPAUSE                  (1u << 3)
#define MAX31329_TMR_TE                      (1u << 4)

// PWR_MGMT bits
#define MAX31329_PWR_DMAN_SEL                (1u << 0)
#define MAX31329_PWR_D_VBACK_SEL             (1u << 1)
#define MAX31329_PWR_PFVT_POS                2
#define MAX31329_PWR_PFVT_MASK               (0x03u << MAX31329_PWR_PFVT_POS)

// TRICKLE bits
#define MAX31329_TRK_D_TRICKLE_POS           0
#define MAX31329_TRK_D_TRICKLE_MASK          (0x0Fu << MAX31329_TRK_D_TRICKLE_POS)
#define MAX31329_TRK_D_TRKCHG_EN             (1u << 7)

#endif // KODE_MAX31329_REGISTERS_H
