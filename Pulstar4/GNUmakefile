# $Id: GNUmakefile,v 1.1.1.1 2004/10/25 12:36:46 kuzniak Exp $
# --------------------------------------------------------------
# GNUmakefile for examples module.  Gabriele Cosmo, 06/04/98.
# --------------------------------------------------------------

name := Pulstar3
G4TARGET := $(name)
G4EXLIB := true

ifndef G4INSTALL
  G4INSTALL = ../../../..
endif

.PHONY: all
all: lib bin

include $(G4INSTALL)/config/architecture.gmk

include $(G4INSTALL)/config/binmake.gmk

########################### ROOT #################################

ifdef ROOTSYS
ifndef G4UI_USE_ROOT
  ROOTCPPFLAGS   = $(shell $(ROOTSYS)/bin/root-config --cflags)
  #ROOTCPPFLAGS  := $(filter-out -D_REENTRANT,$(ROOTCPPFLAGS))
  #ROOTCPPFLAGS  := $(filter-out -pthread,$(ROOTCPPFLAGS))
  CPPFLAGS      += -DG4ANALYSIS_USE_ROOT $(ROOTCPPFLAGS)
  ROOTLIBS       = $(shell $(ROOTSYS)/bin/root-config --nonew --glibs)
  ROOTLIBS      := $(filter-out -lNew,$(ROOTLIBS))
  #ROOTLIBS      := $(filter-out -lThread,$(ROOTLIBS))
  #ROOTLIBS      := $(filter-out -lpthread,$(ROOTLIBS))
  #ROOTLIBS      := $(filter-out -pthread,$(ROOTLIBS))
  ROOTLIBS      += -lMinuit -lHtml
  LDLIBS        += $(ROOTLIBS)
endif
endif