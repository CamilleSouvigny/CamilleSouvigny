############################# LIBRARY IMPORTS  ############################# 

install.packages(c("FactoMineR","factoextra"
,"Factoshiny","datasets"
,"stats","discoveR"
,"RODBC","corrplot"
,"fpc)","doBy"
,"xlsx","plyr"
,"dplyr","data.table"
,"jsonlite","factoextra"
,"corrplot","readr"
,"kohonen","cluster"
,"FactoMineR","ClustOfVar"
,"PCAmixdata","StatMatch"
,"corrplot","Factoshiny"
,"scorecard","ClusterR"
,"dplyr","ggplot2"
,"Rmixmod","dbscan"
,"fpc","mclust"
,"dendextend","magrittr"
,"tidyr","clustree"
,"rpud", "msm"))

library(FactoMineR)
library(factoextra)
library(Factoshiny)
library(datasets)
library(stats)
library(discoveR)
library(RODBC)
library(corrplot)
library(fpc) #cluster boot
library(doBy)
library(xlsx)
library(plyr)
library(dplyr)
library(data.table) # data handling
library(jsonlite)
library(factoextra) # PCA
library(corrplot) # correlation plot
library(readr)
library(kohonen) # SOM
library(cluster) # PAM / CLARA
library(FactoMineR) # PCA
library(ClustOfVar)
library(PCAmixdata)
library(StatMatch)
library(corrplot)
library(Factoshiny)
library(scorecard)
library(ClusterR)
library(dplyr)
library(ggplot2)
library(Rmixmod)
library(dbscan)
library(fpc)
library(mclust)
library(dendextend)
library(magrittr)
library(tidyr)
library(clustree)
library(rpud)     
library(msm)


############################# Data Simulation #################################

size = 10000
SUBSCRIBER_ID <- sample(1:size)
SENIORITY_PLATFORM <- round(runif(n = size, min = 1 , max = 1500 ))
SENIORITY_IN_TC <- list()
for (el in SENIORITY_PLATFORM) {
  SENIORITY_IN_TC <- append(SENIORITY_IN_TC , el - round(runif(n = 1, min = 1 , max = el )* 0.5))
}
SENIORITY_IN_TC <- as.numeric(SENIORITY_IN_TC)

RECENCY_IN_TC <- list()
for (el in SENIORITY_IN_TC) {
  RECENCY_IN_TC <- append(RECENCY_IN_TC, round(runif(n = 1, min = 1 , max = el )))
}

RECENCY_IN_TC <- as.numeric(RECENCY_IN_TC)
NB_TC_WATCHED <- round((SENIORITY_IN_TC - RECENCY_IN_TC)* abs(rnorm(size))/10 ) + 1
NB_DAYS_WATCH_TC <- round(NB_TC_WATCHED * rtnorm(size, lower=0.7, upper=2.2, mean=1, sd=0.7))
TC_CUMULATED_HOURS <- round(NB_TC_WATCHED*2 + NB_TC_WATCHED* rtnorm(size, lower=-1, upper=1, mean=0, sd=1))  
  
AVG_TC_NB_DAYS_FROM_LAUNCHED <- round(rtnorm(size, mean = 50 ,sd = 100, lower=1))
MIN_TC_NB_DAYS_FROM_LAUNCHED <- round(AVG_TC_NB_DAYS_FROM_LAUNCHED - AVG_TC_NB_DAYS_FROM_LAUNCHED* rtnorm(size, mean=0, sd=0.5, lower=0, upper=1) + 1)
MAX_TC_NB_DAYS_FROM_LAUNCHED <- round(AVG_TC_NB_DAYS_FROM_LAUNCHED + AVG_TC_NB_DAYS_FROM_LAUNCHED* rtnorm(size, mean=0, sd=0.5, lower=0, upper=1))

NB_MOVIES <- NB_TC_WATCHED + round(rtnorm(size, mean = 20 ,sd = 20, lower=1))
NB_GENRE <- runif(n = size, min = 1 , max = 10 )
NB_HOURS <- round(NB_MOVIES*2.5 + NB_MOVIES * rtnorm(size, lower=-1, upper=1, mean=0, sd=1))

NB_DAYS_WATCH_ALL <- round(NB_MOVIES * rtnorm(size, 1, lower=0.7, upper=2.2))
TC_ALL_HOUR_RATIO <- TC_CUMULATED_HOURS/NB_HOURS
TC_ALL_HOUR_RATIO_LAST_6_MONTHS <- TC_ALL_HOUR_RATIO + TC_ALL_HOUR_RATIO*rtnorm(size, lower=-1, upper=1, mean=0, sd=1)

  
#hist(rtnorm(size, mean = 40 ,sd = 40, lower=1))
#curve(pnorm(x, mean=10, sd=15), from = 0, to = 100) 
#sample(x=0.2:4, size, replace = TRUE, prob = pnorm(size))


data <- cbind(SUBSCRIBER_ID, SENIORITY_PLATFORM, SENIORITY_IN_TC, RECENCY_IN_TC, 
              NB_TC_WATCHED, NB_DAYS_WATCH_TC, TC_CUMULATED_HOURS, AVG_TC_NB_DAYS_FROM_LAUNCHED,
              MIN_TC_NB_DAYS_FROM_LAUNCHED, MAX_TC_NB_DAYS_FROM_LAUNCHED, NB_MOVIES,
              NB_GENRE, NB_HOURS, NB_DAYS_WATCH_ALL, TC_ALL_HOUR_RATIO, TC_ALL_HOUR_RATIO_LAST_6_MONTHS)

############################# Cleaning check ###################
head(data)
summary(data)
gc()

#Dataset_Clustering = data
#Dataset_Clustering[is.na(Dataset_Clustering)] <- 0
#Dataset_Clustering[Dataset_Clustering<0] <- 0
#summary(Dataset_Clustering)

####################### PCA  ###################################  
PCA_output = prcomp(data[,-1], scale = TRUE)

# Choose nb of axis depending on cumulative variance explained and axis contribution
summary(PCA_output)


fviz_eig(PCA_output)
get_eigenvalue(PCA_output)
get_pca_var(PCA_output)$cos2
get_pca_var(PCA_output)$contrib

PCA_output$contrib

fviz_pca_var(PCA_output,
             col.var = "contrib",
             gradient.cols = c("#00AFBB", "#E7B800", "#FC4E07"),
             repel = TRUE)

fviz_pca_var(PCA_output,
             col.var = "contrib", 
             repel = TRUE, axes=c(3,4))

corrplot(get_pca_var(PCA_output)$cos2, is.corr=FALSE)
corrplot(get_pca_var(PCA_output)$contrib, is.corr=FALSE)

fviz_contrib(PCA_output, choice = "var", axes = 1, top = 10)

datacp <- as.data.frame(PCA_output$x[,  1:7])
dist_acp <- dist(as.matrix(datacp), method = "euclidean")

############################# function : kmeans customised  #################################

kmeans_custo <- function (data, krange, k = NULL, scaling = FALSE, runs = 1, criterion = "ch", ...) {
  if (!is.null(k)) {
    krange <- k
  }
  if (!identical(scaling, FALSE)) {
    sdata <- scale(data, center = TRUE, scale = scaling)
  } else {
    sdata <- data
  }
  c1 <- kmeansruns(sdata, krange, runs = runs, criterion = criterion, iter.max=500, algorithm="MacQueen", ...)
  partition <- c1$cluster
  cl <- list()
  nc <- c1$bestk
  for (i in 1:nc) {
    cl[[i]] <- partition == i
    out <- list(result = c1, nc = nc, clusterlist = cl, partition = partition, 
                clustermethod = "kmeans")
  }
  out
}

####################### Square error partitional  ###################################  
##################################  K-MEANS    #######################################

kms <- vector("list",10)
for (i in 6:10) kms[[i]] <-  clusterboot(datacp, B=150, bootmethod="boot",
                                                  clustermethod=kmeans_custo,
                                                  krange=i, seed=10)
kms[[7]]$bootmean # Most stable nr of centers

nbclust = 7
km.boot_simple = clusterboot(datacp, B=150, bootmethod="boot",
                              clustermethod=kmeans_custo,
                              krange=nbclust, seed=15555)  

km.boot_simple$bootmean ; km.boot_simple$result$result ;km.boot_simple$result$result$size*100/nrow(datacp)

km7=kmeans(datacp , centers=km.boot_simple$result$result$centers)

kmeans7_stat = cluster.stats(dist_acp, km.boot_simple$result$result$cluster)
kmeans7_stat[c("wb.ratio","within.cluster.ss","avg.silwidth","clus.avg.silwidths")]

fviz_cluster(km7, data = datacp, stand = FALSE,
             ellipse = FALSE, show.clust.cent = TRUE,
             geom = "point",palette = "jco", ggtheme = theme_classic())



# CHOOSE K by viewing different metrics

#  within sum of squares
fviz_nbclust(datacp, kmeans, method = "wss", k.max = 24) + theme_minimal() + ggtitle("the Elbow Method")     

# Silhouette
fviz_nbclust(datacp, kmeans, method = "silhouette", k.max = 15) + theme_minimal() + ggtitle("The Silhouette Plot") 

# Gap statistic
gap_stat <- clusGap(datacp, FUN = kmeans, nstart = 30, K.max = 24, B = 50)
fviz_gap_stat(gap_stat) + theme_minimal() + ggtitle("fviz_gap_stat: Gap Statistic")  # How far the clustering structure is from the random uniform distribution of points.

km1 <- kmeans(datacp, 1)
km2 <- kmeans(datacp, 2)
km3 <- kmeans(datacp, 3)
km4 <- kmeans(datacp, 4)
km5 <- kmeans(datacp, 5)
km6 <- kmeans(datacp, 6)
km7 <- kmeans(datacp, 7)
km8 <- kmeans(datacp, 8)
km9 <- kmeans(datacp, 9)
km10 <- kmeans(datacp, 10)
km11 <- kmeans(datacp, 11)
km12 <- kmeans(datacp, 12)
km13 <- kmeans(datacp, 13)
km14 <- kmeans(datacp, 14)
km15 <- kmeans(datacp, 15)
km16 <- kmeans(datacp, 16)
all_km=list(km1, km2, km3, km4, km5, km6, km7, km8, km9, km10, km11, km12, km13, km14, km15, km16)

ssc <- data.frame(
  kmeans = c(2,3,4,5,6,7,8,9,10,11,12,13,14,15,16),
  within_ss = c(mean(km2$withinss), mean(km3$withinss), mean(km4$withinss), mean(km5$withinss), mean(km6$withinss), mean(km7$withinss), mean(km8$withinss),mean(km9$withinss),mean(km10$withinss),mean(km11$withinss),mean(km12$withinss),mean(km13$withinss),mean(km14$withinss),mean(km15$withinss),mean(km16$withinss)),
  between_ss = c(km2$betweenss, km3$betweenss, km4$betweenss, km5$betweenss, km6$betweenss, km7$betweenss, km8$betweenss, km9$betweenss, km10$betweenss, km11$betweenss, km12$betweenss, km13$betweenss, km14$betweenss, km15$betweenss, km16$betweenss)
)
# Look for best wss / bss 
ssc %<>% gather(., key = "measurement", value = value, -kmeans)
ssc %>% ggplot(., aes(x=kmeans, y=value, fill = measurement)) + geom_bar(stat = "identity", position = "dodge") + ggtitle("Cluster Model Comparison") + xlab("Number of Clusters") + ylab("Log10 Total Sum of Squares") + scale_x_discrete(name = "Number of Clusters", limits = c("0", "2", "3", "4", "5", "6", "7", "8","9","10"))

kmax = 16
totwss = rep(0,kmax) 
kmfit = list() # create and empty list
i=0
for (km in all_km){
  i=i+1
  totwss[i] = km$tot.withinss
}
rsq = 1-(totwss*(nrow(datacp)-1))/(totwss[1]*(nrow(datacp)-seq(1,kmax)))  # withinesss / total sum of squares
plot(seq(1,kmax),rsq,xlab="Number of clusters",ylab="Adjusted R-squared",pch=20,cex=2) # look for max increase point



# evaluate AIC : elbow method 
kmeansAIC = function(fit){
  
  m = ncol(fit$centers)
  n = length(fit$cluster)
  k = nrow(fit$centers)
  D = fit$tot.withinss
  return(D + 2*m*k)
}
aic=sapply(all_km,kmeansAIC)
plot(seq(1,16),aic,xlab="Number of clusters",ylab="AIC",pch=20,cex=2) # elbow method



# Clustering trees 
tmp <- NULL
for (k in 1:14){
  tmp[k] <- kmeans(datacp, k, nstart = 5)
}
df <- data.frame(tmp)
colnames(df) <- seq(1:14)
colnames(df) <- paste0("k",colnames(df))

# get individual PCA
df.pca <- prcomp(df, center = TRUE, scale. = FALSE)
ind.coord <- df.pca$x
ind.coord <- ind.coord[,1:2]
df <- bind_cols(as.data.frame(df), as.data.frame(ind.coord))
clustree(df, prefix = "k")
#clustree(df, prefix = "k", node_colour='sc3_stability')

clustree(df, prefix = "k", layout = "sugiyama") # tries to minimize trees crossing

df_subset <- df %>% select(1:8,12:13)
clustree_overlay(df_subset, prefix = "k", x_value = "PC1", y_value = "PC2")

overlay_list <- clustree_overlay(df_subset, prefix = "k", x_value = "PC1",
                                 y_value = "PC2", plot_sides = TRUE)
overlay_list$x_side
overlay_list$y_side


##################################  K-MENOID   ###########################################

fviz_nbclust(datacp, pam, method = "wss", k.max = 14) + theme_minimal() + ggtitle("the Elbow Method")   
fviz_nbclust(datacp, pam, method = "silhouette", k.max = 14) + theme_minimal() + ggtitle("The Silhouette Plot") 

kmenoids = pam(datacp, 7 , metric = 'euclidean')
fviz_cluster(kmenoids, data = df)

#######################  Density functions clustering  ############################# 
##################################  Gaussian Mixture Modelling  #################### 

gm_cl <- Mclust(datacp)
(hc1 <- hc(datacp, modelName = "VVV", use = "SVD"))
(hc2 <- hc(datacp, modelName = "VVV", use = "VARS"))
(hc3 <- hc(datacp, modelName = "EEE", use = "SVD"))
gm_cah1 <- Mclust(datacp, initialization = list(hcPairs = hc1))
gm_cah2 <- Mclust(datacp, initialization = list(hcPairs = hc2))
gm_cah3 <- Mclust(datacp, initialization = list(hcPairs = hc3))

gm_cl$G
gm_cah1$G 
gm_cah2$G
gm_cah3$G

gm_cah3$bic

plot(gm_cl) # plots cluster on each axis combination

gm_cl$modelName # selected model : VVV =  ellipsoidal, varying volume, shape, and orientation
gm_cl$G #  7 clusters

head(gm_cl$z) # probability for an observation to be in a given cluster
summary(gm_cl, parameters = TRUE) #get probabilities, means, variances

gm_stat = cluster.stats(dist_acp, gm_cl$classification)
gm_stat[c("wb.ratio","within.cluster.ss","avg.silwidth","clus.avg.silwidths")]
gm_stat_cah1 = cluster.stats(dist_acp, gm_cah1$classification)
gm_stat_cah1[c("wb.ratio","within.cluster.ss","avg.silwidth","clus.avg.silwidths")]
gm_stat_cah2 = cluster.stats(dist_acp, gm_cah2$classification)
gm_stat_cah2[c("wb.ratio","within.cluster.ss","avg.silwidth","clus.avg.silwidths")]
gm_stat_cah3 = cluster.stats(dist_acp, gm_cah3$classification)
gm_stat_cah3[c("wb.ratio","within.cluster.ss","avg.silwidth","clus.avg.silwidths")]


gm_rs = 1-(gm_stat_cah3$within.cluster.ss*(nrow(datacp)-1))/(totwss[1]*(nrow(datacp)))

fviz_cluster(gm_cl, data = datacp, stand = FALSE,
             ellipse = FALSE, show.clust.cent = TRUE,
             geom = "point",palette = "jco", ggtheme = theme_classic())


gm_cl_bic <- mclustBIC(datacp) # to choose model 
gm_cl_bic
plot(gm_cl_bic)



#LRT <- mclustBootstrapLRT(datacp, modelName = "VVV" , nboot=10)
#LRT$G



#Bootstrap or jackknife estimation of standard errors and percentile bootstrap confidence intervals for the parameters of a Gaussian mixture model.
bootClust <- MclustBootstrap(gm_cah3, nboot=20)
summary(bootClust, what = "se")
summary(bootClust, what = "ci")



density_cluster <- mixmodCluster(datacp, nbCluster=4:10, criterion=c("BIC", "ICL", "NEC"))
#  Gaussian_pk_Lk_C : quantitative data
## Each class is caracterised by its own law of pb. use bayes pb. Generalisation of Kmeans.
density_cluster@bestResult@proba       
density_cluster@bestResult@parameters@variance
plot(density_cluster)

density_cluster@bestResult@criterionValue

gaus_stat = cluster.stats(dist_acp, density_cluster@bestResult@partition )
gaus_stat[c("wb.ratio","within.cluster.ss","avg.silwidth","clus.avg.silwidths")]



##################################  DBSCAN  #################### 

# determine optimal eps value : the average of the distances of every point to its k nearest neighbor
kNNdistplot(datacp_matrix, k=4)
abline(h=1, col="red")

 
tuning_dbscan <- function( data, nbPoints_max=50, epsilon ) {
  result=list()
  for (MinPoint in 2:nbPoints_max) {
    gc()
    print(MinPoint)
    for (eps in epsilon) {
      db.fit = dbscan(datacp_matrix, eps, minPts=MinPoint)
      # consider only clusters with less than 5% of noise points
      if (length(unique(db.fit$cluster))>3 & (sum(db.fit$cluster==0)+1)*100/length(db.fit$cluster)<5) {
          db_cs = cluster.stats(dist_acp, db.fit$cluster)
          print(db_cs$avg.silwidth)
          result[[paste(MinPoint, eps, sep="_")]]= db_cs$avg.silwidth
      }
    }
  }
  result
}

t_dbscan <- tuning_dbscan(datacp_matrix, 50, seq(from = 0.1, to = 15, by = 0.1))

t_dbscan[which.max(t_dbscan)]
# max 4 et 2.3 

length(unique(db$cluster))
(sum(db$cluster==0)+100)*100/length(db$cluster)


db = dbscan(datacp_matrix, 1 , minPts=10)
cluster.stats(dist_acp, db$cluster)
hullplot(datacp_matrix, db$cluster)

db_cs = cluster.stats(dist_acp, db$cluster)

db_cs[c("wb.ratio","within.cluster.ss","avg.silwidth","clus.avg.silwidths")]


fviz_cluster(db, data = datacp, stand = FALSE,
             ellipse = FALSE, show.clust.cent = TRUE,
             geom = "point",palette = "jco", ggtheme = theme_classic())







####################### Hierarchical Clustering    #############

# Regroupement METHODS : 
  # WARD.D : inertia criterion (finding compact, spherical clusters)
  # ward.D2 : sqaured Ward's method
  # complete : maximal distance (creates compact clusters)
  # average : avg distance (tends to regroup clusters with small inertias)
  # single :  minimal distance ('friends of friends' strategy : creates large clusters)
  # median
  # centroid 

fviz_nbclust(datacp, FUN = hcut, method = "wss")
fviz_nbclust(datacp, FUN = hcut, method = "silhouette")


cah.ward2 <- hclust(dist_acp,method="ward.D2")
cah.ward1 <- hclust(dist_acp,method="ward.D")
cah.complete <- hclust(dist_acp,method="complete")
cah.average <- hclust(dist_acp,method="average")
cah.single <- hclust(dist_acp,method="single")
cah.median <- hclust(dist_acp,method="median")
cah.centroid <- hclust(dist_acp,method="centroid")

plot(cah.centroid)

rect.hclust(cah.ward,k=8)
#highlight the groups

sils_hac <- list()
rsq_hac <- list()
for (i in 2:15){
  stat <- cluster.stats(dist_acp, cutree(cah.complete,k=i))
  sils_hac[i] <- stat$avg.silwidth 
  rsq_hac[i] <- stat$within.cluster.ss
    #1-(stat$within.cluster.ss*(nrow(datacp)-1))/(totwss[1]*(nrow(datacp)-i))
}
plot(unlist(rsq_hac))
plot(2:15,unlist(sils_hac))


# best models to keep from HAC
cutree(cah.ward,k=8)
cutree(cah.centroid,k=7)
cutree(cah.median,k=8)
cutree(cah.single,k=6)


#######################  Hierarchical k-means    #############

sils_hk <- list()
wss_hk <- list()
for (i in 2:15){
  hk <- hkmeans(datacp, i ,iter.max = 20)
  sils_hk[i] <- cluster.stats(dist_acp, hk$cluster)$avg.silwidth 
  wss_hk[i] <- 1- (hk$tot.withinss*(nrow(datacp)-1) )/(hk$totss*(nrow(datacp) - i))
}
plot(unlist(wss_hk))
plot(unlist(sils_hk))

hk8= hkmeans(datacp, 8 ,iter.max = 20)

# Visualize the tree
fviz_dend(hk, cex = 0.6, palette = "jco", 
          rect = TRUE, rect_border = "jco", rect_fill = TRUE)



# Visualize the hkmeans final clusters
fviz_cluster(hk, palette = "jco", repel = TRUE,
             ggtheme = theme_classic())














############################# Collaborative clustering  ############################# 

all_cl <- cbind(kmeans7 = km.boot_simple$result$result$cluster,
                kmenoids = kmenoids$clustering,
                gmm = gm_cah3$classification,
                gmm_gaus = density_cluster@bestResult@partition,
                dbscan = db$cluster ,
                hac.ward = cutree(cah.ward,k=8),
                hac.centroid = cutree(cah.centroid,k=7),
                hac.median = cutree(cah.median,k=8),
                hac.single = cutree(cah.single,k68),
                hac.complete = cutree(cah.complete,k=8)
                hk = hk8$cluster
                )


#  number of times points xi and xj were clustered together divided by the total number of times they were selected together.

cm <- consensus_matrix(all_cl)
summary(cm)

dist_cm <- as.dist(1-cm)



cah_cm <- hclust(dist_cm , method="ward.D2")
plot(cah_cm)
rect.hclust(cah_cm,k=8)



sils_cm <- list()
rsq_cm <- list()
for (i in 2:15){
  stat <- cluster.stats(dist_acp, cutree(cah_cm,k=i))
  sils_cm[i] <- stat$avg.silwidth 
  rsq_cm[i] <-  1-(stat$within.cluster.ss*(nrow(datacp)-1))/(totwss[1]*(nrow(datacp)-i))
}
plot(unlist(rsq_cm))
plot(unlist(sils_cm))

cutree(cah_cm,k=6)











