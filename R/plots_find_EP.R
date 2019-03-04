library(lattice)
library(reshape2)
library(dplyr)
library(colorspace)

W_min = -50
W_max = 100
P_min = 0
P_max = 300
L_min = 0
L_max = 300


output_folder <- "C:/Users/morellv/00J/Stefano_2016/mca/Result_160819"

optimal_E <- read.csv(paste(output_folder, "/optimal_E.csv", sep = ""), header = FALSE)
L_grid_size <- ncol(optimal_E)
optimal_E$P <- P_min + ( 1:nrow(optimal_E) - 1) * (P_max - P_min) / (nrow(optimal_E) - 1)
#optimal_E <- rbind(1:ncol(optimal_E), optimal_E)
optimal_E <- melt(optimal_E, id.vars = c("P"))
optimal_E$L <- L_min + ( as.numeric(sub("V", "", optimal_E$variable)) - 1) * (L_max - L_min) / (L_grid_size - 1)
optimal_E <- select(optimal_E %>% rename(E = value), P, L, E)

pdf(paste(output_folder, "/plots/optimal_E.pdf", sep = ""), 9, 8)
levelplot(E ~ L * P, optimal_E, aspect = 1, contour = TRUE, col.regions = diverge_hcl(100))
dev.off()

optimal_D <- read.csv(paste(output_folder, "/optimal_D.csv", sep = ""), header = FALSE)
L_grid_size <- ncol(optimal_D)
optimal_D$P <- P_min + ( 1:nrow(optimal_D) - 1) * (P_max - P_min) / (nrow(optimal_D) - 1)
optimal_D <- melt(optimal_D, id.vars = c("P"))
optimal_D$L <- L_min + ( as.numeric(sub("V", "", optimal_D$variable)) - 1) * (L_max - L_min) / (L_grid_size - 1)
optimal_D <- select(optimal_D %>% rename(D = value), P, L, D)

pdf(paste(output_folder, "/plots/optimal_D.pdf", sep = ""), 9, 8)
levelplot(D ~ L * P, optimal_D, aspect = 1, contour = TRUE, col.regions = diverge_hcl(100))
dev.off()

optimal_equity_W <- read.csv(paste(output_folder, "/optimal_equity_W.csv", sep = ""), header = FALSE)
L_grid_size <- ncol(optimal_equity_W)
optimal_equity_W$P <- P_min + ( 1:nrow(optimal_equity_W) - 1) * (P_max - P_min) / (nrow(optimal_equity_W) - 1)
optimal_equity_W <- melt(optimal_equity_W, id.vars = c("P"))
optimal_equity_W$L <- L_min + ( as.numeric(sub("V", "", optimal_equity_W$variable)) - 1) * (L_max - L_min) / (L_grid_size - 1)
optimal_equity_W <- select(optimal_equity_W %>% rename(equity_W = value), P, L, equity_W)

pdf(paste(output_folder, "/plots/optimal_equity_W.pdf", sep = ""), 9, 8)
levelplot(equity_W ~ L * P, optimal_equity_W, aspect = 1, contour = TRUE, col.regions = diverge_hcl(100),
          at = seq(-1, 1, 0.05))
dev.off()

optimal_equity_L <- read.csv(paste(output_folder, "/optimal_equity_L.csv", sep = ""), header = FALSE)
L_grid_size <- ncol(optimal_equity_L)
optimal_equity_L$P <- P_min + ( 1:nrow(optimal_equity_L) - 1) * (P_max - P_min) / (nrow(optimal_equity_L) - 1)
optimal_equity_L <- melt(optimal_equity_L, id.vars = c("P"))
optimal_equity_L$L <- L_min + ( as.numeric(sub("V", "", optimal_equity_L$variable)) - 1) * (L_max - L_min) / (L_grid_size - 1)
optimal_equity_L <- select(optimal_equity_L %>% rename(equity_L = value), P, L, equity_L)

pdf(paste(output_folder, "/plots/optimal_equity_L.pdf", sep = ""), 9, 8)
levelplot(equity_L ~ L * P, optimal_equity_L, aspect = 1, contour = TRUE, col.regions = diverge_hcl(100),
          at = seq(-1, 1, 0.05))
dev.off()

optimal_I <- read.csv(paste(output_folder, "/optimal_I.csv", sep = ""), header = FALSE)
L_grid_size <- ncol(optimal_I)
optimal_I$P <- P_min + ( 1:nrow(optimal_I) - 1) * (P_max - P_min) / (nrow(optimal_I) - 1)
optimal_I <- melt(optimal_I, id.vars = c("P"))
optimal_I$L <- L_min + ( as.numeric(sub("V", "", optimal_I$variable)) - 1) * (L_max - L_min) / (L_grid_size - 1)
optimal_I <- select(optimal_I %>% rename(I = value), P, L, I)

pdf(paste(output_folder, "/plots/optimal_I.pdf", sep = ""), 9, 8)
levelplot(I ~ L * P, optimal_I, aspect = 1, contour = TRUE, col.regions = diverge_hcl(100),
          at = seq(-1, 5, 0.1))
dev.off()

optimal_W <- read.csv(paste(output_folder, "/optimal_W.csv", sep = ""), header = FALSE)
L_grid_size <- ncol(optimal_W)
optimal_W$P <- P_min + ( 1:nrow(optimal_W) - 1) * (P_max - P_min) / (nrow(optimal_W) - 1)
optimal_W <- melt(optimal_W, id.vars = c("P"))
optimal_W$L <- L_min + ( as.numeric(sub("V", "", optimal_W$variable)) - 1) * (L_max - L_min) / (L_grid_size - 1)
optimal_W <- select(optimal_W %>% rename(W = value), P, L, W)

pdf(paste(output_folder, "/plots/optimal_W.pdf", sep = ""), 9, 8)
levelplot(W ~ L * P, optimal_W, aspect = 1, contour = TRUE, col.regions = diverge_hcl(100))
dev.off()

###################################################

output_folder <- "C:/Users/morellv/00J/Stefano_2016/mca/Result_160823/"

optimal_E <- read.csv(paste(output_folder, "/optimal_E.csv", sep = ""), header = FALSE)
L_grid_size <- ncol(optimal_E)
optimal_E$P <- P_min + ( 1:nrow(optimal_E) - 1) * (P_max - P_min) / (nrow(optimal_E) - 1)
#optimal_E <- rbind(1:ncol(optimal_E), optimal_E)
optimal_E <- melt(optimal_E, id.vars = c("P"))
optimal_E$L <- L_min + ( as.numeric(sub("V", "", optimal_E$variable)) - 1) * (L_max - L_min) / (L_grid_size - 1)
optimal_E <- select(optimal_E %>% rename(E = value), P, L, E)

pdf(paste(output_folder, "/plots/optimal_E.pdf", sep = ""), 9, 8)
levelplot(E ~ L * P, optimal_E, aspect = 1, contour = TRUE, col.regions = diverge_hcl(100))
dev.off()

optimal_D <- read.csv(paste(output_folder, "/optimal_D.csv", sep = ""), header = FALSE)
L_grid_size <- ncol(optimal_D)
optimal_D$P <- P_min + ( 1:nrow(optimal_D) - 1) * (P_max - P_min) / (nrow(optimal_D) - 1)
optimal_D <- melt(optimal_D, id.vars = c("P"))
optimal_D$L <- L_min + ( as.numeric(sub("V", "", optimal_D$variable)) - 1) * (L_max - L_min) / (L_grid_size - 1)
optimal_D <- select(optimal_D %>% rename(D = value), P, L, D)

pdf(paste(output_folder, "/plots/optimal_D.pdf", sep = ""), 9, 8)
levelplot(D ~ L * P, optimal_D, aspect = 1, contour = TRUE, col.regions = diverge_hcl(100))
dev.off()

optimal_equity_W <- read.csv(paste(output_folder, "/optimal_equity_W.csv", sep = ""), header = FALSE)
L_grid_size <- ncol(optimal_equity_W)
optimal_equity_W$P <- P_min + ( 1:nrow(optimal_equity_W) - 1) * (P_max - P_min) / (nrow(optimal_equity_W) - 1)
optimal_equity_W <- melt(optimal_equity_W, id.vars = c("P"))
optimal_equity_W$L <- L_min + ( as.numeric(sub("V", "", optimal_equity_W$variable)) - 1) * (L_max - L_min) / (L_grid_size - 1)
optimal_equity_W <- select(optimal_equity_W %>% rename(equity_W = value), P, L, equity_W)

pdf(paste(output_folder, "/plots/optimal_equity_W.pdf", sep = ""), 9, 8)
levelplot(equity_W ~ L * P, optimal_equity_W, aspect = 1, contour = TRUE, col.regions = diverge_hcl(100),
          at = seq(-1, 1, 0.05))
dev.off()

optimal_equity_L <- read.csv(paste(output_folder, "/optimal_equity_L.csv", sep = ""), header = FALSE)
L_grid_size <- ncol(optimal_equity_L)
optimal_equity_L$P <- P_min + ( 1:nrow(optimal_equity_L) - 1) * (P_max - P_min) / (nrow(optimal_equity_L) - 1)
optimal_equity_L <- melt(optimal_equity_L, id.vars = c("P"))
optimal_equity_L$L <- L_min + ( as.numeric(sub("V", "", optimal_equity_L$variable)) - 1) * (L_max - L_min) / (L_grid_size - 1)
optimal_equity_L <- select(optimal_equity_L %>% rename(equity_L = value), P, L, equity_L)

pdf(paste(output_folder, "/plots/optimal_equity_L.pdf", sep = ""), 9, 8)
levelplot(equity_L ~ L * P, optimal_equity_L, aspect = 1, contour = TRUE, col.regions = diverge_hcl(100),
          at = seq(-1, 1, 0.05))
dev.off()

optimal_I <- read.csv(paste(output_folder, "/optimal_I.csv", sep = ""), header = FALSE)
L_grid_size <- ncol(optimal_I)
optimal_I$P <- P_min + ( 1:nrow(optimal_I) - 1) * (P_max - P_min) / (nrow(optimal_I) - 1)
optimal_I <- melt(optimal_I, id.vars = c("P"))
optimal_I$L <- L_min + ( as.numeric(sub("V", "", optimal_I$variable)) - 1) * (L_max - L_min) / (L_grid_size - 1)
optimal_I <- select(optimal_I %>% rename(I = value), P, L, I)

pdf(paste(output_folder, "/plots/optimal_I.pdf", sep = ""), 9, 8)
levelplot(I ~ L * P, optimal_I, aspect = 1, contour = TRUE, col.regions = diverge_hcl(100),
          at = seq(-1, 5, 0.1))
dev.off()

optimal_W <- read.csv(paste(output_folder, "/optimal_W.csv", sep = ""), header = FALSE)
L_grid_size <- ncol(optimal_W)
optimal_W$P <- P_min + ( 1:nrow(optimal_W) - 1) * (P_max - P_min) / (nrow(optimal_W) - 1)
optimal_W <- melt(optimal_W, id.vars = c("P"))
optimal_W$L <- L_min + ( as.numeric(sub("V", "", optimal_W$variable)) - 1) * (L_max - L_min) / (L_grid_size - 1)
optimal_W <- select(optimal_W %>% rename(W = value), P, L, W)

pdf(paste(output_folder, "/plots/optimal_W.pdf", sep = ""), 9, 8)
levelplot(W ~ L * P, optimal_W, aspect = 1, contour = TRUE, col.regions = diverge_hcl(100))
dev.off()

###################################################

output_folder <- "C:/Users/morellv/00J/Stefano_2016/mca/Results with variant default calculation//"

optimal_E <- read.csv(paste(output_folder, "/optimal_E.csv", sep = ""), header = FALSE)
L_grid_size <- ncol(optimal_E)
optimal_E$P <- P_min + ( 1:nrow(optimal_E) - 1) * (P_max - P_min) / (nrow(optimal_E) - 1)
#optimal_E <- rbind(1:ncol(optimal_E), optimal_E)
optimal_E <- melt(optimal_E, id.vars = c("P"))
optimal_E$L <- L_min + ( as.numeric(sub("V", "", optimal_E$variable)) - 1) * (L_max - L_min) / (L_grid_size - 1)
optimal_E <- select(optimal_E %>% rename(E = value), P, L, E)

pdf(paste(output_folder, "/plots/optimal_E.pdf", sep = ""), 9, 8)
levelplot(E ~ L * P, optimal_E, aspect = 1, contour = TRUE, col.regions = diverge_hcl(100))
dev.off()

optimal_D <- read.csv(paste(output_folder, "/optimal_D.csv", sep = ""), header = FALSE)
L_grid_size <- ncol(optimal_D)
optimal_D$P <- P_min + ( 1:nrow(optimal_D) - 1) * (P_max - P_min) / (nrow(optimal_D) - 1)
optimal_D <- melt(optimal_D, id.vars = c("P"))
optimal_D$L <- L_min + ( as.numeric(sub("V", "", optimal_D$variable)) - 1) * (L_max - L_min) / (L_grid_size - 1)
optimal_D <- select(optimal_D %>% rename(D = value), P, L, D)

pdf(paste(output_folder, "/plots/optimal_D.pdf", sep = ""), 9, 8)
levelplot(D ~ L * P, optimal_D, aspect = 1, contour = TRUE, col.regions = diverge_hcl(100))
dev.off()

optimal_equity_W <- read.csv(paste(output_folder, "/optimal_equity_W.csv", sep = ""), header = FALSE)
L_grid_size <- ncol(optimal_equity_W)
optimal_equity_W$P <- P_min + ( 1:nrow(optimal_equity_W) - 1) * (P_max - P_min) / (nrow(optimal_equity_W) - 1)
optimal_equity_W <- melt(optimal_equity_W, id.vars = c("P"))
optimal_equity_W$L <- L_min + ( as.numeric(sub("V", "", optimal_equity_W$variable)) - 1) * (L_max - L_min) / (L_grid_size - 1)
optimal_equity_W <- select(optimal_equity_W %>% rename(equity_W = value), P, L, equity_W)

pdf(paste(output_folder, "/plots/optimal_equity_W.pdf", sep = ""), 9, 8)
levelplot(equity_W ~ L * P, optimal_equity_W, aspect = 1, contour = TRUE, col.regions = diverge_hcl(100),
          at = seq(-1, 1, 0.05))
dev.off()

optimal_equity_L <- read.csv(paste(output_folder, "/optimal_equity_L.csv", sep = ""), header = FALSE)
L_grid_size <- ncol(optimal_equity_L)
optimal_equity_L$P <- P_min + ( 1:nrow(optimal_equity_L) - 1) * (P_max - P_min) / (nrow(optimal_equity_L) - 1)
optimal_equity_L <- melt(optimal_equity_L, id.vars = c("P"))
optimal_equity_L$L <- L_min + ( as.numeric(sub("V", "", optimal_equity_L$variable)) - 1) * (L_max - L_min) / (L_grid_size - 1)
optimal_equity_L <- select(optimal_equity_L %>% rename(equity_L = value), P, L, equity_L)

pdf(paste(output_folder, "/plots/optimal_equity_L.pdf", sep = ""), 9, 8)
levelplot(equity_L ~ L * P, optimal_equity_L, aspect = 1, contour = TRUE, col.regions = diverge_hcl(100),
          at = seq(-1, 1, 0.05))
dev.off()

optimal_I <- read.csv(paste(output_folder, "/optimal_I.csv", sep = ""), header = FALSE)
L_grid_size <- ncol(optimal_I)
optimal_I$P <- P_min + ( 1:nrow(optimal_I) - 1) * (P_max - P_min) / (nrow(optimal_I) - 1)
optimal_I <- melt(optimal_I, id.vars = c("P"))
optimal_I$L <- L_min + ( as.numeric(sub("V", "", optimal_I$variable)) - 1) * (L_max - L_min) / (L_grid_size - 1)
optimal_I <- select(optimal_I %>% rename(I = value), P, L, I)

pdf(paste(output_folder, "/plots/optimal_I.pdf", sep = ""), 9, 8)
levelplot(I ~ L * P, optimal_I, aspect = 1, contour = TRUE, col.regions = diverge_hcl(100),
          at = seq(-1, 5, 0.1))
dev.off()

optimal_W <- read.csv(paste(output_folder, "/optimal_W.csv", sep = ""), header = FALSE)
L_grid_size <- ncol(optimal_W)
optimal_W$P <- P_min + ( 1:nrow(optimal_W) - 1) * (P_max - P_min) / (nrow(optimal_W) - 1)
optimal_W <- melt(optimal_W, id.vars = c("P"))
optimal_W$L <- L_min + ( as.numeric(sub("V", "", optimal_W$variable)) - 1) * (L_max - L_min) / (L_grid_size - 1)
optimal_W <- select(optimal_W %>% rename(W = value), P, L, W)

pdf(paste(output_folder, "/plots/optimal_W.pdf", sep = ""), 9, 8)
levelplot(W ~ L * P, optimal_W, aspect = 1, contour = TRUE, col.regions = diverge_hcl(100))
dev.off()
