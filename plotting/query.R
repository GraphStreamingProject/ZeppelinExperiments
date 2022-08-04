library(tidyverse)
library("scales")

speed <- read.csv("/home/devd/Programming/git_stuff/stream_data/sigmod2021resub/query.csv")

ggplot() +
  geom_line(data = speed, mapping = aes(x = percent, y = flush+cc, color = system, linetype =system), size = 0.5) +
  #scale_x_continuous(trans="log10") +
  labs(y=expression(paste("Query time (s)")), x ="percent of stream") +
  #scale_y_continuous(trans="log10") +
  #coord_cartesian(ylim = c(0, 3))
  #labs(title = "Ingestion Rate\n", color = "System\n") +
  labs(color = "System\n") +
  #theme(legend.position = c(0.45, -1.23), legend.direction = 'horizontal', plot.margin = grid::unit(c(0,1,2,0), 'lines')) +
  #theme(legend.position = "bottom", legend.direction = 'horizontal', plot.margin = grid::unit(c(1,1,1,1), 'lines')) +
  #theme(legend.position = c(0, -0.05), legend.direction = 'horizontal') +
  
  #guides(color=guide_legend(nrow=2,byrow=TRUE), linetype="none") +
  #guides(linetype="none") +
  #guides(linetype="none") +
  #scale_color_hue(labels = c("Aspen", "GraphZeppelin gutter tree", "GraphZeppelin leaf gutters", "Terrace"))+
  #scale_linetype_manual("", breaks=c("aspen_top", "stream_mem", "terrace", "stream_ext")
  #, values=c("dashed", "solid","longdash","dotdash"))
  scale_color_hue(labels = c("Aspen", "GraphZeppelin", "Terrace"))+
  scale_linetype_manual("", breaks=c("aspen_top", "stream_mem", "terrace")
                        , values=c("dashed", "solid","dotdash"))

ggsave(filename = "query.png", width = 6, height = 2, unit = "in")
