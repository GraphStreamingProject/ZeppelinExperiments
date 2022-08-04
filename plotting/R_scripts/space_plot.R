library(tidyverse)
library("scales")

#size <- read.csv("/home/devd/Programming/git_stuff/stream_data/sigmod2021cam/space_data")
size <- read.csv("space_data.csv")

ggplot() +
  geom_line(data = size, mapping = aes(x = nodes, y = res+swap, color = alg, linetype=alg), size = 0.5) +
  #scale_x_continuous(labels=comma, trans="log10") +
  #ylim(0,100) +
  ylab("maximum data structure size (GiB)") + xlab("number of nodes") +
  #scale_y_continuous(trans="log10")
  #labs(title = "Size of Data Structures\n", color = "System\n") +
  labs(color = "System\n", linetype = "System\n") +
  #theme(legend.position = "bottom") +
  theme(legend.position = c(0.45, -0.20), legend.direction = 'horizontal', plot.margin = grid::unit(c(0,1,2,0), 'lines')) +
  #guides(color=guide_legend(nrow=2,byrow=TRUE), linetype="none") +
  guides(linetype="none") +
  scale_color_hue(labels = c("Aspen", "GraphZeppelin", "Terrace"))+
  scale_linetype_manual("", breaks=c("aspen_top", "stream_mem", "terrace")
                      , values=c("longdash", "solid","dashed"))
  
ggsave(filename = "size.png", width = 6, height = 4, unit = "in")
