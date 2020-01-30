# Load libraries.
library(ggplot2)

# Import new column into dataset:
sarsa_performance <- read.csv("~/Desktop/courses/RLP/RLP2/sarsa_performance.csv", header=T)
sarsa_performance_help <- read.csv("~/Desktop/courses/RLP/RLP2/sarsa_performance_help.csv", header=T)

qlearning_performance <- read.csv("~/Desktop/courses/RLP/RLP2/qlearning_performance.csv", header=T)
qlearning_performance_help <- read.csv("~/Desktop/courses/RLP/RLP2/qlearning_performance_help.csv", header=T)

# Create one dataframe holding all values.
performance <- sarsa_performance
colnames(performance)[which(names(performance) == "reward")] <- "sarsa_reward"

colnames(qlearning_performance)[which(names(qlearning_performance) == "reward")] <- "qlearning_reward"
performance <- merge(performance, qlearning_performance)

colnames(sarsa_performance_help)[which(names(sarsa_performance_help) == "reward")] <- "sarsa_reward_help"
colnames(qlearning_performance_help)[which(names(qlearning_performance_help) == "reward")] <- "qlearning_reward_help"

performance <- merge(performance, sarsa_performance_help)
performance <- merge(performance, qlearning_performance_help)

# Create the plot using all columns.
ggplot(performance, aes(episode)) +
  #geom_line(aes(y=sarsa_reward, colour = "Sarsa")) +
  #geom_line(aes(y=qlearning_reward, colour = "Qlearning")) +
  geom_line(aes(y=sarsa_reward_help, colour = "Sarsa with help")) +
  geom_line(aes(y=qlearning_reward_help, colour = "Qlearning with help")) +
  ylim(-50,100) +
  #scale_y_log10( breaks=c(25,50,100,500,1000,2500,5000), limits=c(25,5000)) +
  ggtitle("Algorithm performance comparison.") +
  theme(plot.title = element_text(hjust = 0.5), legend.position=c(.88,.25)) +
  labs(y = "Reward", x = "Episodes", colour = "Algorithm")

# Create the plot using all columns.
ggplot(performance, aes(episode)) +
  #geom_smooth(aes(y=sarsa_reward, colour = "Sarsa")) +
  #geom_smooth(aes(y=qlearning_reward, colour = "Qlearning")) +
  geom_smooth(aes(y=sarsa_reward_help, colour = "Sarsa with help")) +
  geom_smooth(aes(y=qlearning_reward_help, colour = "Qlearning with help")) +
  ylim(40,75) +
  
  #scale_y_log10( breaks=c(25,50,100,500,1000,2500,5000), limits=c(25,5000)) +
  ggtitle("Algorithm performance comparison.") +
  theme(plot.title = element_text(hjust = 0.5), legend.position=c(.88,.25)) +
  labs(y = "Reward", x = "Episodes", colour = "Algorithm")

# Final report statistics

## Sarsa
# Regular without help
sarsa_performance <- read.csv("~/Desktop/courses/RLP/RLP2/sarsa_performance1.csv", header=T)
for (i in 2:10) {
  temp <- read.csv(sprintf("~/Desktop/courses/RLP/RLP2/sarsa_performance%d.csv", i), header=T)
  for (j in 1:nrow(sarsa_performance)) {
    sarsa_performance[j,] = sarsa_performance[j,] + temp[j,]
  }
}
for (j in 1:nrow(sarsa_performance)) {
  sarsa_performance[j,] = sarsa_performance[j,] / 10
}

# Regular with help
sarsa_performance_help <- read.csv("~/Desktop/courses/RLP/RLP2/sarsa_performance_help1.csv", header=T)
for (i in 2:10) {
  temp <- read.csv(sprintf("~/Desktop/courses/RLP/RLP2/sarsa_performance_help%d.csv", i), header=T)
  for (j in 1:nrow(sarsa_performance_help)) {
    sarsa_performance_help[j,] = sarsa_performance_help[j,] + temp[j,]
  }
}
for (j in 1:nrow(sarsa_performance_help)) {
  sarsa_performance_help[j,] = sarsa_performance_help[j,] / 10
}

# Broken without help
sarsa_performance_broken <- read.csv("~/Desktop/courses/RLP/RLP2/sarsa_performance_broken1.csv", header=T)
for (i in 2:10) {
  temp <- read.csv(sprintf("~/Desktop/courses/RLP/RLP2/sarsa_performance_broken%d.csv", i), header=T)
  for (j in 1:nrow(sarsa_performance_broken)) {
    sarsa_performance_broken[j,] = sarsa_performance_broken[j,] + temp[j,]
  }
}
for (j in 1:nrow(sarsa_performance_broken)) {
  sarsa_performance_broken[j,] = sarsa_performance_broken[j,] / 10
}

# Broken with help
sarsa_performance_help_broken <- read.csv("~/Desktop/courses/RLP/RLP2/sarsa_performance_help_broken1.csv", header=T)
for (i in 2:10) {
  temp <- read.csv(sprintf("~/Desktop/courses/RLP/RLP2/sarsa_performance_help_broken%d.csv", i), header=T)
  for (j in 1:nrow(sarsa_performance_help_broken)) {
    sarsa_performance_help_broken[j,] = sarsa_performance_help_broken[j,] + temp[j,]
  }
}
for (j in 1:nrow(sarsa_performance_help_broken)) {
  sarsa_performance_help_broken[j,] = sarsa_performance_help_broken[j,] / 10
}

## Q-learning
# Regular without help
qlearning_performance <- read.csv("~/Desktop/courses/RLP/RLP2/qlearning_performance1.csv", header=T)
for (i in 2:10) {
  temp <- read.csv(sprintf("~/Desktop/courses/RLP/RLP2/qlearning_performance%d.csv", i), header=T)
  for (j in 1:nrow(qlearning_performance)) {
    qlearning_performance[j,] = qlearning_performance[j,] + temp[j,]
  }
}
for (j in 1:nrow(qlearning_performance)) {
  qlearning_performance[j,] = qlearning_performance[j,] / 10
}

# Regular with help
qlearning_performance_help <- read.csv("~/Desktop/courses/RLP/RLP2/qlearning_performance_help1.csv", header=T)
for (i in 2:10) {
  temp <- read.csv(sprintf("~/Desktop/courses/RLP/RLP2/qlearning_performance_help%d.csv", i), header=T)
  for (j in 1:nrow(qlearning_performance_help)) {
    qlearning_performance_help[j,] = qlearning_performance_help[j,] + temp[j,]
  }
}
for (j in 1:nrow(qlearning_performance_help)) {
  qlearning_performance_help[j,] = qlearning_performance_help[j,] / 10
}

# Broken without help
qlearning_performance_broken <- read.csv("~/Desktop/courses/RLP/RLP2/qlearning_performance_broken1.csv", header=T)
for (i in 2:10) {
  temp <- read.csv(sprintf("~/Desktop/courses/RLP/RLP2/qlearning_performance_broken%d.csv", i), header=T)
  for (j in 1:nrow(qlearning_performance_broken)) {
    qlearning_performance_broken[j,] = qlearning_performance_broken[j,] + temp[j,]
  }
}
for (j in 1:nrow(qlearning_performance_broken)) {
  qlearning_performance_broken[j,] = qlearning_performance_broken[j,] / 10
}

# Broken with help
qlearning_performance_help_broken <- read.csv("~/Desktop/courses/RLP/RLP2/qlearning_performance_help_broken1.csv", header=T)
for (i in 2:10) {
  temp <- read.csv(sprintf("~/Desktop/courses/RLP/RLP2/qlearning_performance_help_broken%d.csv", i), header=T)
  for (j in 1:nrow(qlearning_performance_help_broken)) {
    qlearning_performance_help_broken[j,] = qlearning_performance_help_broken[j,] + temp[j,]
  }
}
for (j in 1:nrow(qlearning_performance_help_broken)) {
  qlearning_performance_help_broken[j,] = qlearning_performance_help_broken[j,] / 10
}

# Create one dataframe holding all values.
performance <- sarsa_performance
colnames(performance)[which(names(performance) == "reward")] <- "sarsa_reward"

colnames(qlearning_performance)[which(names(qlearning_performance) == "reward")] <- "qlearning_reward"
performance <- merge(performance, qlearning_performance)

# Regular help
colnames(sarsa_performance_help)[which(names(sarsa_performance_help) == "reward")] <- "sarsa_reward_help"
colnames(qlearning_performance_help)[which(names(qlearning_performance_help) == "reward")] <- "qlearning_reward_help"

performance <- merge(performance, sarsa_performance_help)
performance <- merge(performance, qlearning_performance_help)

# Broken without help
colnames(sarsa_performance_broken)[which(names(sarsa_performance_broken) == "reward")] <- "sarsa_reward_broken"
colnames(qlearning_performance_broken)[which(names(qlearning_performance_broken) == "reward")] <- "qlearning_reward_broken"

performance <- merge(performance, sarsa_performance_broken)
performance <- merge(performance, qlearning_performance_broken)

# Broken help
colnames(sarsa_performance_help_broken)[which(names(sarsa_performance_help_broken) == "reward")] <- "sarsa_reward_help_broken"
colnames(qlearning_performance_help_broken)[which(names(qlearning_performance_help_broken) == "reward")] <- "qlearning_reward_help_broken"

performance <- merge(performance, sarsa_performance_help_broken)
performance <- merge(performance, qlearning_performance_help_broken)

# Create the plot using all columns.
ggplot(performance, aes(episode)) +
  geom_smooth(aes(y=sarsa_reward, colour = "Sarsa")) +
  geom_smooth(aes(y=qlearning_reward, colour = "Qlearning")) +
  geom_smooth(aes(y=sarsa_reward_help, colour = "Sarsa with help")) +
  geom_smooth(aes(y=qlearning_reward_help, colour = "Qlearning with help")) +
  geom_smooth(aes(y=sarsa_reward_broken, colour = "Sarsa and broken")) +
  geom_smooth(aes(y=qlearning_reward_broken, colour = "Qlearning and broken")) +
  geom_smooth(aes(y=sarsa_reward_help_broken, colour = "Sarsa with help and broken")) +
  geom_smooth(aes(y=qlearning_reward_help_broken, colour = "Qlearning with help and broken")) +
  ylim(40,90) +
  
  #scale_y_log10( breaks=c(25,50,100,500,1000,2500,5000), limits=c(25,5000)) +
  ggtitle("Algorithm performance comparison.") +
  theme(plot.title = element_text(hjust = 0.5), legend.position=c(.88,.25)) +
  labs(y = "Reward", x = "Episodes", colour = "Algorithm")

# Create the plot using all columns.
ggplot(performance, aes(episode)) +
  geom_line(aes(y=sarsa_reward, colour = "Sarsa")) +
  geom_line(aes(y=qlearning_reward, colour = "Qlearning")) +
  geom_line(aes(y=sarsa_reward_help, colour = "Sarsa with help")) +
  geom_line(aes(y=qlearning_reward_help, colour = "Qlearning with help")) +
  geom_line(aes(y=sarsa_reward_broken, colour = "Sarsa and broken")) +
  geom_line(aes(y=qlearning_reward_broken, colour = "Qlearning and broken")) +
  geom_line(aes(y=sarsa_reward_help_broken, colour = "Sarsa with help and broken")) +
  geom_line(aes(y=qlearning_reward_help_broken, colour = "Qlearning with help and broken")) +
  ylim(40,90) +
  
  #scale_y_log10( breaks=c(25,50,100,500,1000,2500,5000), limits=c(25,5000)) +
  ggtitle("Algorithm performance comparison.") +
  theme(plot.title = element_text(hjust = 0.5), legend.position=c(.88,.25)) +
  labs(y = "Reward", x = "Episodes", colour = "Algorithm")

# For the report we want to compare regular and broken mazes separately, so we make two separate plots:

# Regular maze
ggplot(performance, aes(episode)) +
  geom_smooth(aes(y=sarsa_reward, colour = "Sarsa")) +
  geom_smooth(aes(y=qlearning_reward, colour = "Qlearning")) +
  geom_smooth(aes(y=sarsa_reward_help, colour = "Sarsa with help")) +
  geom_smooth(aes(y=qlearning_reward_help, colour = "Qlearning with help"))
  #ylim(40,90)

# Broken maze
ggplot(performance, aes(episode)) +
  geom_smooth(aes(y=sarsa_reward_broken, colour = "Sarsa")) +
  geom_smooth(aes(y=qlearning_reward_broken, colour = "Qlearning")) +
  geom_smooth(aes(y=sarsa_reward_help_broken, colour = "Sarsa with help")) +
  geom_smooth(aes(y=qlearning_reward_help_broken, colour = "Qlearning with help")) +
  ylim(0,80)

# Help vs no help
srs1 = t.test(performance$sarsa_reward, performance$sarsa_reward_help)
ql1 = t.test(performance$qlearning_reward, performance$qlearning_reward_help)
srs2 = t.test(performance$sarsa_reward_broken, performance$sarsa_reward_help_broken)
ql2 = t.test(performance$qlearning_reward_broken, performance$qlearning_reward_help_broken)

# Sarsa vs q-learning
no_help1 = t.test(performance$sarsa_reward, performance$qlearning_reward)
help1 = t.test(performance$sarsa_reward_help, performance$qlearning_reward_help)
no_help2 = t.test(performance$sarsa_reward_broken, performance$qlearning_reward_broken)
help2 = t.test(performance$sarsa_reward_help_broken, performance$qlearning_reward_help_broken)

p = c(srs1$p.value, ql1$p.value, srs2$p.value, ql2$p.value)
help_p = c(no_help1$p.value, help1$p.value, no_help2$p.value, help2$p.value)
p.adjust(p, method = "bonferroni")
p.adjust(help_p, method = "bonferroni")
#???
p.adjust(c(help_p, p), method = "bonferroni")
  