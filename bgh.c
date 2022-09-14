/*
 * bgh.c
 *
 * by Alfie Kirkpatrick, 2021
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 3 or later of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * $Id: text.c,v 1.114 2017/04/04 04:49:32 plm Exp $
 */

#include "config.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <glib.h>
#include "glib-ext.h"

#include "backgammon.h"
#include "drawboard.h"
#include "export.h"
#include "format.h"
#include "positionid.h"
#include "relational.h"
#include "formatgs.h"

static const char *aszBghRating[N_RATINGS] = {
        N_("harmless"), // awful
        N_("mostly_harmless"), // beginner
        N_("poor"), // casual
        N_("average"), // intermediate
        N_("competent"), // advanced
        N_("dangerous"), // expert
        N_("deadly"), // world class
        N_("elite"), // supernatural
        N_("na")
};

static const char *aszBghLuckRating[N_LUCKS] = {
        N_("very_unlucky"),
        N_("unlucky"),
        N_("none"),
        N_("lucky"),
        N_("very_lucky"),
};

static const char *aszBghSkillType[] = {
        N_("very_bad"),
        N_("bad"),
        N_("doubtful"),
        N_("okay"),
};


/*
 * Print html header for board: move or cube decision 
 *
 * Input:
 *   pf: output file
 *   ms: current match state
 *   iMove: move no.
 *
 */

static const char *aszBghPlayerStr[] = {"B", "W"};

extern void
BghMoveIntro(GString *gsz, const matchstate *pms, const int UNUSED(iGame), const int iMove) {
//    if (iMove >= 0)
//        g_string_append_printf(gsz, _("M%d:"), iMove + 1);

    if (pms->fResigned)
        /* resignation */
        g_string_append_printf(gsz, _("Resign:%d"), pms->fResigned * pms->nCube);
//        g_string_append_printf(gsz,
//                               ngettext(" %s resigns %d point",
//                                        " %s resigns %d points",
//                                        pms->fResigned * pms->nCube),
//                               ap[pms->fTurn].szName, pms->fResigned * pms->nCube);

    else if (pms->anDice[0] && pms->anDice[1])
        g_string_append_printf(gsz, _("%u%u"), pms->anDice[0], pms->anDice[1]);
//    else if (pms->fDoubled)
    /* take decision */
//        g_string_append(gsz, "--TAKE--");
//        g_string_append_printf(gsz, _("Double:%d"), pms->nCube * 2);
//    else
//        g_string_append_printf(gsz, _(" %s on roll, cube decision?\n\n"), ap[pms->fMove].szName);
}


/*
 * Print html header: dtd, head etc.
 *
 * Input:
 *   pf: output file
 *   ms: current match state
 *
 */

extern void
BghPrologue(GString *gsz, const matchstate *pms, const int UNUSED(iGame)) {

    g_string_append_printf(gsz, "\n\n%d:%d:%d:%d\n", pms->cGames, pms->anScore[0], pms->anScore[1], pms->bgv);
//    g_string_append_printf(gsz, pms->cGames == 1 ?
//                                _("The score (after %d game) is: %s %d, %s %d") :
//                                _("The score (after %d games) is: %s %d, %s %d"),
//                           pms->cGames, ap[0].szName, pms->anScore[0], ap[1].szName, pms->anScore[1]);
//
//    if (pms->nMatchTo > 0) {
//        g_string_append_printf(gsz,
//                               ngettext(" (match to %d point)", " (match to %d points)", pms->nMatchTo), pms->nMatchTo);
//        if (pms->fCrawford)
//            g_string_append(gsz, _(", Crawford game"));
//        if (pms->fPostCrawford)
//            g_string_append(gsz, _(", post-Crawford play"));
//    }

//    g_string_append(gsz, "\n");

}


/*
 * Print html header: dtd, head etc.
 *
 * Input:
 *   pf: output file
 *   ms: current match state
 *
 */

//static void
//BghEpilogue(FILE *pf, const matchstate *UNUSED(pms)) {
//
//    time_t t;
//
//    const char szVersion[] = "$Revision: 1.114 $";
//    int iMajor, iMinor;
//
//    iMajor = atoi(strchr(szVersion, ' '));
//    iMinor = atoi(strchr(szVersion, '.') + 1);
//
//    time(&t);
//
//    fprintf(pf, _("Output generated %s" "by %s "), ctime(&t), VERSION_STRING);
//
//    fprintf(pf, _("(Text Export version %d.%d)\n\n"), iMajor, iMinor);
//
//}

char *
OutputCubeAnalysisBgh(float aarOutput[2][NUM_ROLLOUT_OUTPUTS],
                      float aarStdDev[2][NUM_ROLLOUT_OUTPUTS], const evalsetup *pes, const cubeinfo *pci) {

    static char sz[4096];
    int i;
    float arDouble[4];
    const char *aszCube[] = {
            NULL,
            N_("no_double"),
            N_("double_take"),
            N_("double_pass")
    };

    int ai[3];
    cubedecision cd;
    float r;

    FindCubeDecision(arDouble, aarOutput, pci);

    sprintf(sz, "");

    /* Output percentags for evaluations */

    if (exsExport.fCubeDetailProb && pes->et == EVAL_EVAL) {
        strcat(sz, OutputPercents(aarOutput[0], TRUE));
    }

    getCubeDecisionOrdering(ai, arDouble, aarOutput, pci);

    for (i = 0; i < 3; i++) {
//        sprintf(strchr(sz, 0), ":"/*, gettext(aszCube[ai[i]])*/);

        double eq = arDouble[ai[i]];
        double mwc = eq2mwc(eq, pci);
        double mwc_diff = mwc - eq2mwc(arDouble[OUTPUT_OPTIMAL], pci);
//        strcat(sz, );
        sprintf(strchr(sz, 0), ":\n  %d %s %s %.4f %.4f", ai[i], OutputEquity(eq, pci, TRUE),
                OutputEquityDiff(eq, arDouble[OUTPUT_OPTIMAL], pci), mwc, mwc_diff);
    }

    /* cube decision */

    cd = FindBestCubeDecision(arDouble, aarOutput, pci);

    sprintf(strchr(sz, 0), ":\n  %s", GetCubeRecommendation(cd));

    if ((r = getPercent(cd, arDouble)) >= 0.0)
        sprintf(strchr(sz, 0), " (%.3f)", 100.0f * r);

    return sz;
}

#define SKILL_DESC(v) (v == SKILL_NONE ? "okay" : aszSkillType[v])

char *
OutputCubeAnalysisFullBgh(float aarOutput[2][NUM_ROLLOUT_OUTPUTS],
                          float aarStdDev[2][NUM_ROLLOUT_OUTPUTS],
                          const evalsetup *pes, const cubeinfo *pci,
                          int fDouble, int fTake, skilltype stDouble, skilltype stTake) {

    float r;

    int fMissed;
    int fAnno = FALSE;

    float arDouble[4];

    static char sz[4096];

    strcpy(sz, "");
    strcat(sz, "  #\n  ");

    /* check if cube analysis should be printed */

    if (pes->et == EVAL_NONE)
        return NULL;            /* no evaluation */

    FindCubeDecision(arDouble, aarOutput, pci);

    fMissed = fDouble > -1 && isMissedDouble(arDouble, aarOutput, fDouble, pci);

    if (fMissed) {
        fAnno = TRUE;

        sprintf(strchr(sz, 0), "%s:%s",
                aszBghSkillType[stDouble],
                OutputEquityDiff(arDouble[OUTPUT_NODOUBLE],
                                 (arDouble[OUTPUT_TAKE] >
                                  arDouble[OUTPUT_DROP]) ? arDouble[OUTPUT_DROP] : arDouble[OUTPUT_TAKE], pci));
    }

    r = arDouble[OUTPUT_TAKE] - arDouble[OUTPUT_DROP];

    if (fTake > 0 && r > 0.0f) {
        fAnno = TRUE;
        sprintf(strchr(sz, 0), "%s:%s", aszBghSkillType[stTake],
                OutputEquityDiff(arDouble[OUTPUT_DROP], arDouble[OUTPUT_TAKE], pci));
    }

    r = arDouble[OUTPUT_DROP] - arDouble[OUTPUT_TAKE];

    if (fDouble > 0 && !fTake && r > 0.0f) {
        fAnno = TRUE;

        sprintf(strchr(sz, 0), "%s:%s", aszBghSkillType[stTake],
                OutputEquityDiff(arDouble[OUTPUT_TAKE], arDouble[OUTPUT_DROP], pci));
    }


    if (arDouble[OUTPUT_TAKE] > arDouble[OUTPUT_DROP])
        r = arDouble[OUTPUT_NODOUBLE] - arDouble[OUTPUT_DROP];
    else
        r = arDouble[OUTPUT_NODOUBLE] - arDouble[OUTPUT_TAKE];

    if (fDouble > 0 && fTake < 0 && r > 0.0f) {
        fAnno = TRUE;

        /* wrong double */
        sprintf(strchr(sz, 0), "%s:%s", aszBghSkillType[stDouble],
                OutputEquityDiff((arDouble[OUTPUT_TAKE] >
                                  arDouble[OUTPUT_DROP]) ?
                                 arDouble[OUTPUT_DROP] : arDouble[OUTPUT_TAKE], arDouble[OUTPUT_NODOUBLE], pci));
    }

    if (!fAnno) {
        strcat(sz, "okay:0");
    }

//    if ((badSkill(stDouble) || badSkill(stTake)) && !fAnno) {
//
//        if (badSkill(stDouble)) {
//            sprintf(strchr(sz, 0), _("XXXAlert: double decision marked %s"), gettext(aszSkillType[stDouble]));
//            strcat(sz, "\n");
//        }
//
//        if (badSkill(stTake)) {
//            sprintf(strchr(sz, 0), _("XXXAlert: take decision marked %s"), gettext(aszSkillType[stTake]));
//            strcat(sz, "\n");
//        }
//
//    }

    strcat(sz, ":");
    strcat(sz, OutputCubeAnalysisBgh(aarOutput, aarStdDev, pes, pci));
    strcat(sz, "\n");

    return sz;

}

/*
 * Print cube analysis
 *
 * Input:
 *  pf: output file
 *  arDouble: equitites for cube decisions
 *  fPlayer: player who doubled
 *  esDouble: eval setup
 *  pci: cubeinfo
 *  fDouble: double/no double
 *  fTake: take/drop
 *
 */

static void
BghPrintCubeAnalysisTable(GString *gsz,
                          float aarOutput[2][NUM_ROLLOUT_OUTPUTS],
                          float aarStdDev[2][NUM_ROLLOUT_OUTPUTS],
                          int UNUSED(fPlayer),
                          const evalsetup *pes, const cubeinfo *pci,
                          int fDouble, int fTake, skilltype stDouble, skilltype stTake) {

    int fActual, fClose, fMissed;
//    int fDisplay;
    float arDouble[4];

    /* check if cube analysis should be printed */

    if (pes->et == EVAL_NONE)
        return;                 /* no evaluation */

    FindCubeDecision(arDouble, aarOutput, pci);

    fActual = fDouble;
    fClose = isCloseCubedecision(arDouble);
    fMissed = isMissedDouble(arDouble, aarOutput, fDouble, pci);

//    fDisplay =
//            (fActual && exsExport.afCubeDisplay[EXPORT_CUBE_ACTUAL]) ||
//            (fClose && exsExport.afCubeDisplay[EXPORT_CUBE_CLOSE]) ||
//            (fMissed && exsExport.afCubeDisplay[EXPORT_CUBE_MISSED]) ||
//            (exsExport.afCubeDisplay[stDouble]) || (exsExport.afCubeDisplay[stTake]);

//    if (!fDisplay)
//        return;

    g_string_append(gsz, OutputCubeAnalysisFullBgh(aarOutput, aarStdDev, pes, pci, fDouble, fTake, stDouble, stTake));
}


/*
 * Wrapper for print cube analysis
 *
 * Input:
 *  pf: output file
 *  pms: match state
 *  pmr: current move record
 *  szImageDir: URI to images
 *  szExtension: extension of images
 *  fTake: take/drop
 *
 */

static void
BghPrintCubeAnalysis(GString *gsz, const matchstate *pms, moverecord *pmr) {

    cubeinfo ci;
    /* we need to remember the double type to be able to do the right
     * thing for beavers and racoons */
//    static doubletype dt = DT_NORMAL;

    GetMatchStateCubeInfo(&ci, pms);


    switch (pmr->mt) {

        case MOVE_NORMAL:
            /* cube analysis from move */
            BghPrintCubeAnalysisTable(gsz,
                                      pmr->CubeDecPtr->aarOutput,
                                      pmr->CubeDecPtr->aarStdDev,
                                      pmr->fPlayer, &pmr->CubeDecPtr->esDouble, &ci, FALSE, -1, pmr->stCube,
                                      SKILL_NONE);
//            dt = DT_NORMAL;
            break;

        case MOVE_DOUBLE:
//            dt = DoubleType(pms->fDoubled, pms->fMove, pms->fTurn);
//            if (dt != DT_NORMAL) {
//                g_string_append(gsz, _("Cannot analyse beaver nor raccoons!\n"));
//                break;
//            }
            BghPrintCubeAnalysisTable(gsz,
                                      pmr->CubeDecPtr->aarOutput,
                                      pmr->CubeDecPtr->aarStdDev,
                                      pmr->fPlayer, &pmr->CubeDecPtr->esDouble, &ci, TRUE, -1, pmr->stCube, SKILL_NONE);
            break;

        case MOVE_TAKE:
        case MOVE_DROP:
            /* cube analysis from double, {take, drop, beaver} */
//            if (dt != DT_NORMAL) {
//                dt = DT_NORMAL;
//                g_string_append(gsz, _("Cannot analyse beaver nor raccoons!\n"));
//                break;
//            }
            BghPrintCubeAnalysisTable(gsz, pmr->CubeDecPtr->aarOutput, pmr->CubeDecPtr->aarStdDev, pmr->fPlayer,
                                      &pmr->CubeDecPtr->esDouble, &ci, TRUE, pmr->mt == MOVE_TAKE,
                                      SKILL_NONE,  /* FIXME: skill from prev. cube */
                                      pmr->stCube);

            break;

        default:
            g_assert_not_reached();
    }
}


/*
 * Print move analysis
 *
 * Input:
 *  pf: output file
 *  pms: match state
 *  pmr: current move record
 *  szImageDir: URI to images
 *  szExtension: extension of images
 *
 */

static void
BghPrintMoveAnalysis(GString *gsz, const matchstate *pms, moverecord *pmr) {
    char sz[64];
    unsigned int i;

    cubeinfo ci;

    GetMatchStateCubeInfo(&ci, pms);

    g_string_append_printf(gsz, ":%s", aszBghLuckRating[pmr->lt]);
    g_string_append_printf(gsz, ":%0.3f", pmr->rLuck);
    g_string_append_printf(gsz, ":%0.3f", 100.0f * (eq2mwc(pmr->rLuck, &ci) - eq2mwc(0.0f, &ci)));

    if (pmr->ml.cMoves > 0) {
        g_string_append_printf(gsz, ":%s", aszBghSkillType[pmr->n.stMove]);

//        if (!pms->nMatchTo || !fOutputMWC)
//            g_string_append_printf(gsz, ":%.3f",
//                                   pmr->ml.amMoves[pmr->n.iMove].rScore - pmr->ml.amMoves[0].rScore);
////        else
//        g_string_append_printf(gsz, ":%.3f",
//                               100.0f *
//                               eq2mwc(pmr->ml.amMoves[pmr->n.iMove].rScore, &ci) -
//                               100.0f * eq2mwc(pmr->ml.amMoves[0].rScore, &ci));
//    } else {
//        g_string_append(gsz, ":-");
    }

    g_string_append(gsz, "\n  #\n");

    if (pmr->ml.cMoves) {

        for (i = 0; i < pmr->ml.cMoves; i++) {
            if (i >= exsExport.nMoves && i != pmr->n.iMove)
                continue;

            g_string_append(gsz, i == pmr->n.iMove ? "* " : "  ");
//            g_string_append(gsz, FormatMoveHint(szBuf, pms, &pmr->ml, i,
//                                                i != pmr->n.iMove ||
//                                                i != pmr->ml.cMoves - 1 ||
//                                                pmr->ml.cMoves == 1 ||
//                                                i < exsExport.nMoves,
//                                                exsExport.fMovesDetailProb,
//                                                exsExport.afMovesParameters[pmr->ml.amMoves[i].esMove.et - 1]));

            char szTemp[2048], szMove[32];
//            int fRankKnown=i != pmr->n.iMove ||
//                           i != pmr->ml.cMoves - 1 ||
//                           pmr->ml.cMoves == 1 ||
//                           i < exsExport.nMoves;
//            int fShowParameters = exsExport.afMovesParameters[pmr->ml.amMoves[i].esMove.et - 1];
            float *ar, *arStdDev;
            float rEq, rEqTop;
            movelist *pml = &pmr->ml;

//            strcpy(szBuf, "");

            /* number */

//            if (i && !fRankKnown)
//                strcat(szBuf, "   ??  ");
//            else
//                sprintf(strchr(szBuf, 0), " %4i. ", i + 1);

            /* eval */

//            sprintf(strchr(szBuf, 0),
//                    "%-14s   %-28s %s: ",
//                    FormatEval(szTemp, &pml->amMoves[i].esMove),
//                    FormatMove(szMove, pms->anBoard,
//                               pml->amMoves[i].anMove), (!pms->nMatchTo || !fOutputMWC) ? _("Eq.") : _("MWC"));
            g_string_append(gsz, FormatMove(szMove, pms->anBoard, pml->amMoves[i].anMove));
//                    , (!pms->nMatchTo || !fOutputMWC) ? _("Eq.") : _("MWC"));

            /* equity or mwc for move */

            ar = pml->amMoves[i].arEvalMove;
            arStdDev = pml->amMoves[i].arEvalStdDev;
            rEq = pml->amMoves[i].rScore;
            rEqTop = pml->amMoves[0].rScore;

            g_string_append_printf(gsz, ":%s:%.3f", OutputEquity(rEq, &ci, FALSE), 100.0f * eq2mwc(rEq, &ci));
//            g_string_append(gsz, ":");
//            g_string_append(gsz, OutputEquity(rEq, &ci, TRUE));
//            strcat(szBuf, );

            /* difference */

//            if (i)
//                sprintf(strchr(szBuf, 0), " (%s)\n", OutputEquityDiff(rEq, rEqTop, &ci));
//            else
//                strcat(szBuf, "\n");

            /* percentages */

            if (exsExport.fMovesDetailProb) {
                switch (pml->amMoves[i].esMove.et) {
                    case EVAL_EVAL:
                        g_string_append_printf(gsz, ":%s", OutputPercents(ar, TRUE));
//                        strcat(szBuf, ":");
//                        strcat(szBuf, OutputPercents(ar, TRUE));
//                        strcat(szBuf, "\n");
                        break;
//                    case EVAL_ROLLOUT:
//                        strcat(szBuf, OutputRolloutResult(":", NULL, (float (*)[NUM_ROLLOUT_OUTPUTS])
//                                ar, (float (*)[NUM_ROLLOUT_OUTPUTS])
//                                                               arStdDev, &ci, 0, 1, pml->amMoves[i].esMove.rc.fCubeful));
//                        break;
                    default:
                        break;

                }
            }

            /* eval parameters */

//            if (1) {
//                switch (pml->amMoves[i].esMove.et) {
//                    case EVAL_EVAL:
//                        g_string_append(gsz, OutputEvalContext(&pml->amMoves[i].esMove.ec, TRUE));
////                        strcat(szBuf, "        ");
////                        strcat(szBuf, OutputEvalContext(&pml->amMoves[i].esMove.ec, TRUE));
////                        strcat(szBuf, "\n");
//                        break;
//                    case EVAL_ROLLOUT:
////                        strcat(szBuf, OutputRolloutContext("        ", &pml->amMoves[i].esMove.rc));
//                        break;
//
//                    default:
//                        break;
//                }
//            }
//            g_string_append_printf(gsz, "%s", szBuf);

            g_string_append(gsz, "\n");
        }

//    } else {
//
//        if (pmr->n.anMove[0] >= 0)
//            /* no movelist saved */
//            g_string_append_printf(gsz, "*    %s\n", FormatMove(sz, pms->anBoard, pmr->n.anMove));
//        else
//            /* no legal moves */
//            /* FIXME: output equity?? */
//            g_string_append_printf(gsz, "* %s\n", _("Cannot move"));
//
    }

//    g_string_append(gsz, "\n");
}


/*
 * Print cube analysis
 *
 * Input:
 *  pf: output file
 *  pms: match state
 *  pmr: current move record
 *
 */

extern void
BghAnalysis(GString *gsz, const matchstate *pms, moverecord *pmr) {


    char sz[1024];

    switch (pmr->mt) {

        case MOVE_NORMAL:

//            if (pmr->n.anMove[0] >= 0)
//                g_string_append_printf(gsz,
//                                       _("%s"),
//                                       FormatMove(sz, pms->anBoard, pmr->n.anMove));
//            else if (!pmr->ml.cMoves)
//                g_string_append_printf(gsz, _("* %s cannot move"), ap[pmr->fPlayer].szName);

//            g_string_append(gsz, "\n");

            if (exsExport.fIncludeAnalysis) {
                BghPrintMoveAnalysis(gsz, pms, pmr);
                BghPrintCubeAnalysis(gsz, pms, pmr);
            }

            break;

        case MOVE_DOUBLE:
        case MOVE_TAKE:
        case MOVE_DROP:

            if (pmr->mt == MOVE_DOUBLE)
                g_string_append(gsz, "Double");
            else if (pmr->mt == MOVE_TAKE)
                g_string_append(gsz, "Take");
            else
                g_string_append(gsz, "Drop");
//                g_string_append_printf(gsz,
//                                       "* %s %s\n\n",
//                                       ap[pmr->fPlayer].szName, (pmr->mt == MOVE_TAKE) ? _("accepts") : _("rejects"));

            if (exsExport.fIncludeAnalysis)
                BghPrintCubeAnalysis(gsz, pms, pmr);

            break;

        default:
            break;

    }

}

static void formatPlay(GString *gsz, const statcontext *psc, float aaaar[3][2][2][2], int type) {
    static char *szType[4] = {"CP", "CB", "OV"};
    int n;

    g_string_append_printf(gsz, "\n>%s", szType[type]);
    for (int i = 1; i >= 0; --i) {
        // Error total EMG (MWC)
        g_string_append_printf(gsz, ":%.3f:%.3f", -aaaar[type][TOTAL][i][NORMALISED],
                               -aaaar[type][TOTAL][i][UNNORMALISED] * 100);

        // Error rate mEMG (MWC)
        g_string_append_printf(gsz, ":%.3f:%.3f", -aaaar[type][PERMOVE][i][NORMALISED] * 1000,
                               -aaaar[type][PERMOVE][i][UNNORMALISED] * 100);

        // Error rate snowie
        if ((n = psc->anTotalMoves[0] + psc->anTotalMoves[1]) > 0)
            g_string_append_printf(gsz, ":%.3f", (float) aaaar[type][TOTAL][i][NORMALISED] / (float) n * -1000);
        else
            g_string_append(gsz, ":na");

        // Text rating
        if (psc->anUnforcedMoves[i])
            g_string_append_printf(gsz, ":%s",
                                   aszBghRating[GetRating(aaaar[type][PERMOVE][i][NORMALISED])]);
        else
            g_string_append(gsz, ":na");
    }
}


static void
BghDumpStatcontext(GString *gsz, const statcontext *psc, int nMatchTo) {
    float aaaar[3][2][2][2];
    getMWCFromError(psc, aaaar);

    // @see formatGS for all examples
    // NOTE that gnubg has white/black not black/white as bgh has it
    if (psc->fMoves) {
        formatPlay(gsz, psc, aaaar, CHEQUERPLAY);
        formatPlay(gsz, psc, aaaar, CUBEDECISION);
        formatPlay(gsz, psc, aaaar, COMBINED);

        g_string_append(gsz, "\n>LK");
        for (int i = 1; i >= 0; --i) {
            g_string_append_printf(gsz, ":%.3f:%.3f", psc->arLuck[i][0], psc->arLuck[i][1] * 100);
            g_string_append_printf(gsz, ":%.3f:%.3f",
                                   (float) psc->arLuck[i][0] / (float) psc->anTotalMoves[i] * 1000,
                                   psc->arLuck[i][1] / (float) psc->anTotalMoves[i] * 100);
            if (psc->anTotalMoves[i])
                g_string_append_printf(gsz, ":%s", aszBghLuckRating[getLuckRating(
                        psc->arLuck[i][0] / (float) psc->anTotalMoves[i])]);
            else
                g_string_append(gsz, ":na");
        }

        g_string_append(gsz, "\n>FB");
        for (int i = 1; i >= 0; --i) {
            if (psc->anCloseCube[i] + psc->anUnforcedMoves[i])
                g_string_append_printf(gsz, ":%.1f", absoluteFibsRating(aaaar[CHEQUERPLAY][PERMOVE][i][NORMALISED],
                                                                        aaaar[CUBEDECISION][PERMOVE][i][NORMALISED],
                                                                        nMatchTo, rRatingOffset));
            else
                g_string_append(gsz, ":0");
        }

    }
    g_string_append(gsz, "\n\n");

}


static void
BghPrintComment(FILE *pf, const moverecord *pmr) {

    char *sz = pmr->sz;


    if (sz) {

        fputs(_("Annotation:\n"), pf);
        fputs(sz, pf);
        fputs("\n", pf);

    }


}

/*
 * Export a game in BGH format
 *
 * Input:
 *   pf: output file
 *   plGame: list of moverecords for the current game
 *
 */

static void ExportGameText(FILE *pf, listOLD *plGame, const int iGame, const int fLastGame) {
    listOLD *pl;
    moverecord *pmr;
    matchstate msExport;
    matchstate msOrig;
    int iMove = 0;
    statcontext *psc = NULL;
    static statcontext scTotal;
    xmovegameinfo *pmgi = NULL;
    GString *gsz;
    listOLD *pl_hint = NULL;
    statcontext *psc_rel;

    msOrig.nMatchTo = 0;

    if (!iGame)
        IniStatcontext(&scTotal);

    updateStatisticsGame(plGame);

    if (game_is_last(plGame))
        pl_hint = game_add_pmr_hint(plGame);

    for (pl = plGame->plNext; pl != plGame; pl = pl->plNext) {
        pmr = pl->p;

        FixMatchState(&msExport, pmr);

        switch (pmr->mt) {

            case MOVE_GAMEINFO:

                ApplyMoveRecord(&msExport, plGame, pmr);

                gsz = g_string_new(NULL);
                BghPrologue(gsz, &msExport, iGame);
                fputs(gsz->str, pf);
                g_string_free(gsz, TRUE);

                msOrig = msExport;
                pmgi = &pmr->g;

                psc = &pmr->g.sc;

                AddStatcontext(psc, &scTotal);
                break;

            case MOVE_NORMAL:

                if (pmr->fPlayer != msExport.fMove) {
                    SwapSides(msExport.anBoard);
                    msExport.fMove = pmr->fPlayer;
                }

                msExport.fTurn = msExport.fMove = pmr->fPlayer;
                msExport.anDice[0] = pmr->anDice[0];
                msExport.anDice[1] = pmr->anDice[1];

                gsz = g_string_new(NULL);
                g_string_append_printf(gsz, "\n~%s:", aszBghPlayerStr[pmr->fPlayer]);
                BghMoveIntro(gsz, &msExport, iGame, iMove);
                fputs(gsz->str, pf);
                g_string_free(gsz, TRUE);

                gsz = g_string_new(NULL);
                BghAnalysis(gsz, &msExport, pmr);
                fputs(gsz->str, pf);
                g_string_free(gsz, TRUE);

                iMove++;

                break;

            case MOVE_DOUBLE:
            case MOVE_TAKE:
            case MOVE_DROP:

                gsz = g_string_new(NULL);
                g_string_append_printf(gsz, "\n~%s:", aszBghPlayerStr[pmr->fPlayer]);
                BghMoveIntro(gsz, &msExport, iGame, iMove);
                fputs(gsz->str, pf);
                g_string_free(gsz, TRUE);

                gsz = g_string_new(NULL);
                BghAnalysis(gsz, &msExport, pmr);
                fputs(gsz->str, pf);
                g_string_free(gsz, TRUE);

                iMove++;

                break;

            default:

                break;

        }

        if (exsExport.fIncludeAnnotation)
            BghPrintComment(pf, pmr);

        ApplyMoveRecord(&msExport, plGame, pmr);

    }

    if (pl_hint)
        game_remove_pmr_hint(pl_hint);

//    if (pmgi && pmgi->fWinner != -1) {
//        /* print game result */
//        fprintf(pf,
//                ngettext("%s wins %d point", "%s wins %d points",
//                         pmgi->nPoints), ap[pmgi->fWinner].szName, pmgi->nPoints);
//    }

    if (psc) {
        gsz = g_string_new(NULL);
        BghDumpStatcontext(gsz, psc, msOrig.nMatchTo);
        g_string_append(gsz, "\n$\n");
        fputs(gsz->str, pf);
        g_string_free(gsz, TRUE);
    }

    if (fLastGame) {
        gsz = g_string_new(NULL);
//        g_string_append(gsz, "\n");
        BghDumpStatcontext(gsz, &scTotal, msOrig.nMatchTo);
        fputs(gsz->str, pf);
        g_string_free(gsz, TRUE);
    }

//    if (fLastGame) {
//        gsz = g_string_new(NULL);
//        if (msOrig.nMatchTo)
//            g_string_append_printf(gsz, _("Match statistics\n\n"));
//        else
//            g_string_append_printf(gsz, _("Session statistics\n\n"));
//        BghDumpStatcontext(gsz, &scTotal, msOrig.nMatchTo);
//
//        psc_rel = relational_player_stats_get(ap[0].szName, ap[1].szName);
//        if (psc_rel) {
//            g_string_append_printf(gsz, _("\nStatistics from database\n\n"));
//            BghDumpStatcontext(gsz, psc_rel, 0);
//            g_free(psc_rel);
//        }
//        fputs(gsz->str, pf);
//        g_string_free(gsz, TRUE);
//    }
}

extern void CommandExportMatchBackgammonHub(char *sz) {
    FILE *pf;
    listOLD *pl;
    int nGames;
    char *szCurrent;
    int i;

    sz = NextToken(&sz);

    if (!sz || !*sz) {
        outputl(_("You must specify a file to export to (see `help export " "match text')."));
        return;
    }

    /* Find number of games in match */
    for (pl = lMatch.plNext, nGames = 0; pl != &lMatch; pl = pl->plNext, nGames++);

    szCurrent = filename_from_iGame(sz, 0);
    setDefaultFileName(sz);
    if (!strcmp(szCurrent, "-"))
        pf = stdout;
    else if ((pf = gnubg_g_fopen(szCurrent, "w")) == 0) {
        outputerr(szCurrent);
        g_free(szCurrent);
        return;
    }

    for (pl = lMatch.plNext, i = 0; pl != &lMatch; pl = pl->plNext, i++) {
        ExportGameText(pf, pl->p, i, i == nGames - 1);
    }
    if (pf != stdout)
        fclose(pf);

    g_free(szCurrent);

}
