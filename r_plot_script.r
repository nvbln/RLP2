# Load libraries.
library(ggplot2)

# Import new column into dataset:
sarsa_performance <- read.csv("~/AI3/Reinforcement Learning/RLP2/sarsa_performance.csv", header=T)
sarsa_performance_help <- read.csv("~/AI3/Reinforcement Learning/RLP2/sarsa_performance_help.csv", header=T)

qlearning_performance <- read.csv("~/AI3/Reinforcement Learning/RLP2/qlearning_performance.csv", header=T)
qlearning_performance_help <- read.csv("~/AI3/Reinforcement Learning/RLP2/qlearning_performance_help.csv", header=T)

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
  geom_line(aes(y=sarsa_reward, colour = "Sarsa")) +
  geom_line(aes(y=qlearning_reward, colour = "Qlearning")) +
  geom_line(aes(y=sarsa_reward_help, colour = "Sarsa with help")) +
  geom_line(aes(y=qlearning_reward_help, colour = "Qlearning with help")) +
  ylim(-50,100) +
  #scale_y_log10( breaks=c(25,50,100,500,1000,2500,5000), limits=c(25,5000)) +
  ggtitle("Algorithm performance comparison.") +
  theme(plot.title = element_text(hjust = 0.5), legend.position=c(.88,.25)) +
  labs(y = "Number of steps", x = "Episodes", colour = "Algorithm")
