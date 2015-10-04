package com.supplyframe.partslookup;

import java.sql.Timestamp;
import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.InputStreamReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.io.DataInput;
import java.io.DataOutput;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.net.URI;
import java.util.Iterator;
import java.util.List;
import java.util.ArrayList;
import java.util.LinkedList;
import java.util.Calendar;
import java.util.Properties;
import java.util.Date;
import java.util.Map;
import java.util.Map.Entry;
import java.util.HashMap;
import java.util.Stack;
import java.util.HashSet;
import java.util.Set;
import java.text.SimpleDateFormat;
import java.text.ParseException;
import org.apache.commons.lang.StringUtils;
import org.apache.hadoop.conf.Configured;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.FileUtil;
import org.apache.hadoop.fs.FSDataInputStream;
import org.apache.hadoop.fs.FSDataOutputStream;
import org.apache.hadoop.fs.LocatedFileStatus;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.fs.RemoteIterator;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.io.FloatWritable;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.WritableComparable;
import org.apache.hadoop.io.WritableComparator;
import org.apache.hadoop.io.compress.SnappyCodec;
import org.apache.hadoop.filecache.DistributedCache;
import org.apache.hadoop.mapreduce.InputFormat;
import org.apache.hadoop.mapreduce.InputSplit;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.JobContext;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.Partitioner;
import org.apache.hadoop.mapreduce.RecordReader;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mapreduce.TaskAttemptContext;
import org.apache.hadoop.mapreduce.lib.input.MultipleInputs;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.TextInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.SequenceFileAsBinaryOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.SequenceFileOutputFormat;
import org.apache.hadoop.mapreduce.lib.partition.InputSampler;
import org.apache.hadoop.mapreduce.lib.partition.TotalOrderPartitioner;
import org.apache.hadoop.util.Tool;
import org.apache.hadoop.util.ToolRunner;
import org.apache.hadoop.util.bloom.BloomFilter;
import org.apache.hadoop.util.bloom.Key;
import org.apache.hadoop.util.hash.Hash;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
* This Hadoop job does a global sort of the database
*/

public class Sorter
extends Configured
implements Tool{
	Properties userProp = null;
	public Sorter(Properties userProp){
		this.userProp = userProp;
	}

	public static int main(String args[]){
		Properties userProp = new Properties();
		try{
	        userProp.load(new FileReader(args[0]));
	        Utility.debug("Loaded property file");
	        ToolRunner.run(new Sorter(userProp),args);
		}catch(Exception ex){
			System.err.println("Cannot load the property file "+args[0]);
			return 1;
		}
		return 0;
	}

	@Override
	public int run(String [] args)
	throws Exception{
		System.out.println("Will sort, normalize and index data");
		int numReduceTasks = Integer.parseInt(userProp.getProperty("SHARDS"));
		double pcnt = 10.0;
		int numSamples = numReduceTasks;
		int maxSplits = numReduceTasks - 1;
		if (0 >= maxSplits) {
			  maxSplits = Integer.MAX_VALUE;
		}
		InputSampler.Sampler<String,String> sampler = new InputSampler.RandomSampler(pcnt, numSamples, maxSplits);
		Job job = Job.getInstance(getConf(), "Parts Lookup Dictionary Sorter");
		String pathStr = userProp.getProperty("SORT_INPUT");
		FileInputFormat.addInputPath(job, new Path(pathStr));
		FileSystem.get(getConf()).delete(new Path(userProp.getProperty("SORT_OUTPUT")));
		FileOutputFormat.setOutputPath(job,new Path(userProp.getProperty("SORT_OUTPUT")));
		job.setJarByClass(Pipeline.class);
		job.setInputFormatClass(SortInputFormat.class);

		job.setMapperClass(Mapper.class);
		job.setMapOutputKeyClass(Text.class);
		job.setMapOutputValueClass(Text.class);
		job.setReducerClass(Reducer.class);
		job.setOutputKeyClass(Text.class);
		job.setOutputValueClass(Text.class);
		job.setNumReduceTasks(numReduceTasks);
		job.setPartitionerClass(TotalOrderPartitioner.class);
		TotalOrderPartitioner.setPartitionFile(job.getConfiguration(),new Path("partition"));
		InputSampler.writePartitionFile(job,sampler);
		int retValue = job.waitForCompletion(true) ? 0 : 1;
		return retValue;
	}


	private static  class SortInputFormat
	extends InputFormat {

		private final TextInputFormat inputFormat = new TextInputFormat();

		@Override
		public List<InputSplit> getSplits(JobContext context)
			throws IOException, InterruptedException {
			return inputFormat.getSplits(context);
		}

		@Override
		public RecordReader<Text, Text> createRecordReader(final InputSplit genericSplit, TaskAttemptContext context)
			throws IOException, InterruptedException {
			context.setStatus(genericSplit.toString());
			return new SortRecordReader(inputFormat.createRecordReader(genericSplit, context));
		}
	}

	private static class SortRecordReader
	extends RecordReader<Text, Text> {

		/* methods are delegated to this variable
		 *
		 */
		private final RecordReader<LongWritable, Text> reader;

		//private final Text mpnText = new Text();
		//private final Text dataSourceText = new Text();
		//private final Text dateText = new Text();
		private final Text currentKey = new Text();
		private final Text currentValue = new Text();

		public SortRecordReader(final RecordReader<LongWritable, Text> reader)
		throws IOException {
			this.reader = reader;
		}

		@Override
		public void close() throws IOException {
			reader.close();
		}

		@Override
		public Text getCurrentKey() {
			return currentKey;
		}

		@Override
		public Text getCurrentValue() {
			return currentValue;
		}

		@Override
		public float getProgress() throws IOException, InterruptedException {	  
			return reader.getProgress();
		}

		@Override	public void initialize(InputSplit split, TaskAttemptContext context)
		throws IOException, InterruptedException {
			reader.initialize(split, context);
		}

		@Override
		public boolean nextKeyValue()
		throws IOException, InterruptedException {
			boolean result = reader.nextKeyValue();
			if (!result) {
				return false;
			}
			Text lineRecordReaderValue = reader.getCurrentValue();
			currentKey.set(extractKey(lineRecordReaderValue));
			currentValue.set(extractValue(lineRecordReaderValue));
			return true;
		}

		private String extractKey(final Text value)
			throws IOException {
			String[] parts = value.toString().split("\t");
			return parts[0];
			//System.err.println("Extracted key "+parts[0]+Constants.FIELD_DELIM+parts[1]+Constants.FIELD_DELIM+parts[2]);
			//currentKey.setMpnStr(parts[0]);
			//currentKey.setDataSourceStr(parts[1]);
			//currentKey.setDateStr(parts[2]);
			//return newKey;
		}

		private String extractValue(final Text value)
		throws IOException {
			String[] tokens = value.toString().split(Constants.FIELD_DELIM);
			return StringUtils.join(tokens,Constants.FIELD_DELIM,1,tokens.length);
		}
	}
}
