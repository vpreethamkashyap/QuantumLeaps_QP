//////////////////////////////////////////////////////////////////////////////
// Model: game.qm
// File:  ./qm_code/missile.cpp
//
// This file has been generated automatically by QP Modeler (QM).
// DO NOT EDIT THIS FILE MANUALLY.
//
// Please visit www.state-machine.com/qm for more information.
//////////////////////////////////////////////////////////////////////////////
#include "qp_port.h"
#include "bsp.h"
#include "game.h"

//Q_DEFINE_THIS_FILE

// encapsulated delcaration of the Missile active object ---------------------
// $(AOs::Missile) ...........................................................
class Missile : public QActive {
private:
    uint8_t m_x;
    uint8_t m_y;
    uint8_t m_exp_ctr;

public:
    Missile() : QActive((QStateHandler)&Missile::initial) {
    }

protected:
    static QState initial(Missile *me, QEvent const *e);
    static QState armed(Missile *me, QEvent const *e);
    static QState flying(Missile *me, QEvent const *e);
    static QState exploding(Missile *me, QEvent const *e);
};

// local objects -------------------------------------------------------------
static Missile l_missile;    // the sole instance of the Missile active object

// Public-scope objects ------------------------------------------------------
QActive * const AO_Missile = (QActive *)&l_missile;          // opaque pointer

// Active object definition --------------------------------------------------
// $(AOs::Missile) ...........................................................

// $(AOs::Missile::Statechart) ...............................................
// @(/2/2/4/0)
QState Missile::initial(Missile *me, QEvent const *e) {
    me->subscribe(TIME_TICK_SIG);

    // object dictionary for Missile object...
    QS_OBJ_DICTIONARY(&l_missile);

    // dictionaries for Missile HSM...
    QS_FUN_DICTIONARY(&Missile::initial);
    QS_FUN_DICTIONARY(&Missile::armed);
    QS_FUN_DICTIONARY(&Missile::flying);
    QS_FUN_DICTIONARY(&Missile::exploding);

    // local signals...
    QS_SIG_DICTIONARY(MISSILE_FIRE_SIG,   &l_missile);
    QS_SIG_DICTIONARY(HIT_WALL_SIG,       &l_missile);
    QS_SIG_DICTIONARY(DESTROYED_MINE_SIG, &l_missile);
    return Q_TRAN(&Missile::armed);
}
// $(AOs::Missile::Statechart::armed) ........................................
QState Missile::armed(Missile *me, QEvent const *e) {
    switch (e->sig) {
        // @(/2/2/4/1/0)
        case MISSILE_FIRE_SIG: {
            // initialize position from the Ship
            me->m_x = ((ObjectPosEvt const *)e)->x;
            me->m_y = ((ObjectPosEvt const *)e)->y;
            return Q_TRAN(&Missile::flying);
        }
    }
    return Q_SUPER(&QHsm::top);
}
// $(AOs::Missile::Statechart::flying) .......................................
QState Missile::flying(Missile *me, QEvent const *e) {
    switch (e->sig) {
        // @(/2/2/4/2/0)
        case TIME_TICK_SIG: {
            ObjectImageEvt *oie;
            // @(/2/2/4/2/0/0)
            if (me->m_x + GAME_MISSILE_SPEED_X < GAME_SCREEN_WIDTH) {
                me->m_x += GAME_MISSILE_SPEED_X;
                // tell the Tunnel to draw the Missile and test for wall hits
                oie = Q_NEW(ObjectImageEvt, MISSILE_IMG_SIG);
                oie->x   = me->m_x;
                oie->y   = me->m_y;
                oie->bmp = MISSILE_BMP;
                AO_Tunnel->postFIFO(oie);
                return Q_HANDLED();
            }
            // @(/2/2/4/2/0/1)
            else {
                return Q_TRAN(&Missile::armed);
            }
        }
        // @(/2/2/4/2/1)
        case HIT_WALL_SIG: {
            return Q_TRAN(&Missile::exploding);
        }
        // @(/2/2/4/2/2)
        case DESTROYED_MINE_SIG: {
            // tell the Ship the score for destroing this Mine
            AO_Ship->postFIFO(e);
            return Q_TRAN(&Missile::armed);
        }
    }
    return Q_SUPER(&QHsm::top);
}
// $(AOs::Missile::Statechart::exploding) ....................................
QState Missile::exploding(Missile *me, QEvent const *e) {
    switch (e->sig) {
        // @(/2/2/4/3)
        case Q_ENTRY_SIG: {
            me->m_exp_ctr = 0;
            return Q_HANDLED();
        }
        // @(/2/2/4/3/0)
        case TIME_TICK_SIG: {
            // @(/2/2/4/3/0/0)
            if ((me->m_x >= GAME_SPEED_X) && (me->m_exp_ctr < 15)) {
                ObjectImageEvt *oie;

                ++me->m_exp_ctr;  // advance the explosion counter
                me->m_x -= GAME_SPEED_X; // move the explosion by one step

                // tell the Tunnel to render the current stage of Explosion
                oie = Q_NEW(ObjectImageEvt, EXPLOSION_SIG);
                oie->x   = me->m_x + 3; // x-pos of explosion
                oie->y   = (int8_t)((int)me->m_y - 4); // y-pos
                oie->bmp = EXPLOSION0_BMP + (me->m_exp_ctr >> 2);
                AO_Tunnel->postFIFO(oie);
                return Q_HANDLED();
            }
            // @(/2/2/4/3/0/1)
            else {
                return Q_TRAN(&Missile::armed);
            }
        }
    }
    return Q_SUPER(&QHsm::top);
}

